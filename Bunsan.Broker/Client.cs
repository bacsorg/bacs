using Bunsan.Broker.Rabbit;
using ProtoBuf;
using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using RabbitMQ.Client.Exceptions;
using RabbitMQ.Client.MessagePatterns;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace Bunsan.Broker
{
    public class Client : IDisposable
    {
        private class Reader : DefaultBasicConsumer
        {
            public delegate void MessageCallback(BasicDeliverEventArgs message);
            public delegate void ReconnectCallback();

            private readonly MessageCallback message_callback;
            private readonly ErrorCallback error_callback;
            private readonly ReconnectCallback reconnect_callback;

            public Reader(IModel model, 
                          MessageCallback message_callback, 
                          ErrorCallback error_callback,
                          ReconnectCallback reconnect_callback)
                : base(model)
            {
                this.message_callback = message_callback;
                this.error_callback = error_callback;
                this.reconnect_callback = reconnect_callback;
            }

            private void call_message(object message_object)
            {
                var message = (BasicDeliverEventArgs)message_object;
                try
                {
                    message_callback(message);
                }
                catch (Exception e)
                {
                    try
                    {
                        error_callback(message.BasicProperties.CorrelationId, e.ToString());
                    }
                    catch (Exception) { /* ignore */ }
                }
            }

            private void call_reconnect(object ignored)
            {
                reconnect_callback();
            }

            public override void HandleBasicDeliver(string consumerTag, 
                                                    ulong deliveryTag, 
                                                    bool redelivered, 
                                                    string exchange, 
                                                    string routingKey, 
                                                    IBasicProperties properties, 
                                                    byte[] body)
            {
                var message = new BasicDeliverEventArgs
                {
                    ConsumerTag = consumerTag,
                    DeliveryTag = deliveryTag,
                    Redelivered = redelivered,
                    Exchange = exchange,
                    RoutingKey = routingKey,
                    BasicProperties = properties,
                    Body = body,
                };
                ThreadPool.QueueUserWorkItem(new WaitCallback(call_message), message);
            }

            public override void HandleModelShutdown(object model, ShutdownEventArgs reason)
            {
                ThreadPool.QueueUserWorkItem(new WaitCallback(call_reconnect));
            }
        };

        private readonly ConnectionFactory connection_factory;
        private bool running = true;
        private IConnection connection;
        private IModel channel;
        private string status_queue;
        private string result_queue;
        private string error_queue;
        private Reader status_reader;
        private Reader result_reader;
        private Reader error_reader;

        public Client(ConnectionParameters parameters)
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
            if (string.IsNullOrEmpty(parameters.Identifier)) throw new ArgumentException("Expected non-empty Identifier");
            status_queue = "client." + parameters.Identifier + ".status";
            result_queue = "client." + parameters.Identifier + ".result";
            error_queue = "client." + parameters.Identifier + ".error";
        }

        public delegate void StatusCallback(string id, Status status);
        public delegate void ResultCallback(string id, Result result);
        public delegate void ErrorCallback(string id, string error);

        public void Listen(StatusCallback status_callback, 
                           ResultCallback result_callback, 
                           ErrorCallback error_callback)
        {
            status_reader = new Reader(channel, (message) =>
            {
                // we don't care about error handling of Status messages
                using (var stream = new MemoryStream(message.Body))
                {
                    var status = Serializer.Deserialize<RabbitStatus>(stream);
                    // note: message was already acked, exceptions can be ignored
                    status_callback(status.Identifier, status.Status);
                }
            }, error_callback, reconnect);
            result_reader = new Reader(channel, (message) =>
            {
                using (var stream = new MemoryStream(message.Body))
                {
                    RabbitResult result = null;
                    try
                    {
                        result = Serializer.Deserialize<RabbitResult>(stream);
                    }
                    catch (Exception e)
                    {
                        // no reason to keep invalid message
                        channel.BasicNack(message.DeliveryTag, false, false);
                        throw;
                    }
                    result_callback(result.Identifier, result.Result);
                }
                // commit phase
                channel.BasicAck(message.DeliveryTag, false);
            }, error_callback, reconnect);
            error_reader = new Reader(channel, (message) =>
            {
                error_callback(message.BasicProperties.CorrelationId,
                               System.Text.Encoding.UTF8.GetString(message.Body));
                // commit phase
                channel.BasicAck(message.DeliveryTag, false);
            }, error_callback, reconnect);
            reconnect();
        }

        private void reconnect()
        {
            lock (connection_factory)
            {
                if (!running) return;
                while (connection == null || !connection.IsOpen)
                {
                    try
                    {
                        connect();
                    }
                    catch (BrokerUnreachableException)
                    {
                        Thread.Sleep(5000);
                    }
                }
            }
        }

        private void connect()
        {
            connection = connection_factory.CreateConnection();
            channel = connection.CreateModel();
            channel.QueueDeclare(queue: status_queue, durable: false, exclusive: false, autoDelete: true, arguments: null);
            channel.QueueDeclare(queue: result_queue, durable: true, exclusive: false, autoDelete: false, arguments: null);
            channel.QueueDeclare(queue: error_queue, durable: true, exclusive: false, autoDelete: false, arguments: null);
            channel.BasicConsume(queue: status_queue, noAck: true, consumer: status_reader);
            channel.BasicConsume(queue: result_queue, noAck: false, consumer: result_reader);
            channel.BasicConsume(queue: error_queue, noAck: false, consumer: error_reader);
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
            var properties = channel.CreateBasicProperties();
            properties.ReplyTo = error_queue;
            properties.CorrelationId = id;
            properties.DeliveryMode = 2;  // persistent
            for (; ; )
            {
                try
                {
                    lock (connection_factory)
                    {
                        if (!running) throw new InvalidOperationException("Already closed");
                        channel.BasicPublish(exchange: "", 
                                             routingKey: constraints.Resource[0], 
                                             basicProperties: properties, 
                                             body: data);
                    }
                    break;
                }
                catch (AlreadyClosedException)
                {
                    reconnect();
                }
            }
        }

        public void Close()
        {
            lock (connection_factory)
            {
                running = false;
                if (connection != null && connection.IsOpen)
                    connection.Close();
            }
        }

        public void Dispose()
        {
            Close();
        }
    }
}
