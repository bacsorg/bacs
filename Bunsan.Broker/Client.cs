using Bunsan.Broker.Rabbit;
using ProtoBuf;
using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using RabbitMQ.Client.MessagePatterns;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace Bunsan.Broker
{
    class Client : IDisposable
    {
        class Reader
        {
            public delegate void Callback(BasicDeliverEventArgs message);

            public Reader(Subscription subscription)
            {
                this.subscription = subscription;
                callback = null;
                consumer = null;
            }

            public void Start(Callback callback)
            {
                if (callback == null) throw new ArgumentException("Callback can't be null");
                lock (locker)
                {
                    if (this.callback != null) throw new ArgumentException("Already started");
                    if (consumer != null) throw new ArgumentException("Invalid Reader state (probably Terminate() is runnning)");
                    this.callback = callback;
                    consumer = new Thread(new ThreadStart(Run));
                    consumer.Start();
                }
            }

            public void Terminate()
            {
                lock (locker)
                {
                    callback = null;
                }
                consumer.Join();
                lock (locker)
                {
                    consumer = null;
                }
            }

            private void Run()
            {
                for (;;)
                {
                    BasicDeliverEventArgs message;
                    bool result = subscription.Next(100, out message);
                    Callback callback = null;
                    // capture callback or terminate
                    lock (locker)
                    {
                        if (this.callback != null)
                        {
                            callback = this.callback;
                        }
                        else
                        {
                            return;
                        }
                    }
                    // safely call delegate
                    try
                    {
                        if (result) callback(message);
                    }
                    catch (Exception)
                    {
                        // TODO log exception
                    }
                }
            }

            private Subscription subscription;
            private Callback callback;
            private Thread consumer;
            private object locker = new object();
        }

        private ConnectionFactory connection_factory;
        private IConnection connection;
        private IModel channel;
        private string status_queue;
        private string result_queue;
        private Reader status_reader;
        private Reader result_reader;

        Client(ConnectionParameters parameters)
        {
            connection_factory = new ConnectionFactory();
            if (parameters.Host != null) connection_factory.HostName = parameters.Host;
            if (parameters.Port != 0) connection_factory.Port = parameters.Port;
            if (parameters.VirtualHost != null) connection_factory.VirtualHost = parameters.VirtualHost;
            if (parameters.Credentials != null)
            {
                var credentials = parameters.Credentials;
                if (credentials.UserName != null) connection_factory.UserName = credentials.UserName;
                if (credentials.Password != null) connection_factory.Password = credentials.Password;
            }
            connection = connection_factory.CreateConnection();
            channel = connection.CreateModel();
            status_queue = parameters.Identifier + "_status";
            result_queue = parameters.Identifier + "_result";
            channel.QueueDeclare(queue: status_queue, durable: true, exclusive: false, autoDelete: false, arguments: null);
            channel.QueueDeclare(queue: result_queue, durable: false, exclusive: false, autoDelete: true, arguments: null);
            status_reader = new Reader(new Subscription(model: channel, queueName: status_queue, noAck: true));
            result_reader = new Reader(new Subscription(model: channel, queueName: result_queue, noAck: false));
        }

        public delegate void StatusCallback(string id, Status status);
        public delegate void ResultCallback(string id, Result result);

        public void Listen(StatusCallback status_callback, ResultCallback result_callback)
        {
            status_reader.Start((message) => {
                // we don't care about error handling of Status messages
                using (var stream = new MemoryStream(message.Body))
                {
                    var status = Serializer.Deserialize<RabbitStatus>(stream);
                    status_callback(status.Identifier, status.Status);
                }
            });
            result_reader.Start((message) =>
            {
                using (var stream = new MemoryStream(message.Body))
                {
                    RabbitResult result = null;
                    try
                    {
                        result = Serializer.Deserialize<RabbitResult>(stream);
                    }
                    catch (ProtoBuf.ProtoException)
                    {
                        // no reason to keep invalid message
                        channel.BasicAck(message.DeliveryTag, false);
                        throw;
                    }
                    result_callback(result.Identifier, result.Result);
                }
                // commit phase
                channel.BasicAck(message.DeliveryTag, false);
            });
        }

        public void Terminate()
        {
            status_reader.Terminate();
            result_reader.Terminate();
        }

        public void Send(Constraints constraints, string id, Task task)
        {
            RabbitTask rabbit_task = new RabbitTask()
            {
                Identifier = id,
                Task = task,
                Constraints = constraints,
                ResultQueue = result_queue,
                StatusQueue = status_queue,
            };
            byte[] data = null;
            using (var stream = new MemoryStream())
            {
                Serializer.Serialize(stream, rabbit_task);
                data = new byte[stream.Length];
                stream.Seek(0, SeekOrigin.Begin);
                stream.Read(data, 0, data.Length);
            }
            if (constraints.Resource.Length != 1)
                throw new NotImplementedException("Supports only single resource constraint");
            channel.BasicPublish(exchange: "", routingKey: constraints.Resource[0], basicProperties: null, body: data);
        }

        public void Dispose()
        {
            Terminate();
        }
    }
}
