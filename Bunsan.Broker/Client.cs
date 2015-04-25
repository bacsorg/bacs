using Bunsan.Broker.Rabbit;
using ProtoBuf;
using RabbitMQ.Client;
using RabbitMQ.Client.MessagePatterns;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    class Client : IDisposable
    {
        private ConnectionFactory connection_factory;
        private IConnection connection;
        private IModel channel;
        private string status_queue;
        private string result_queue;
        private Subscription status_subscription;
        private Subscription result_subscription;

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
            status_subscription = new Subscription(model: channel, queueName: status_queue, noAck: true);
            result_subscription = new Subscription(model: channel, queueName: result_queue, noAck: false);
        }

        public delegate void StatusCallback(string id, Status status);
        public delegate void ResultCallback(string id, Result result);

        public void Listen(StatusCallback status_callback, ResultCallback result_callback)
        {
            throw new NotImplementedException();
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
            throw new NotImplementedException();
        }
    }
}
