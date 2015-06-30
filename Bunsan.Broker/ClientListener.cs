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
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    public class ClientListener : ClientBase
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

        private Reader status_reader;
        private Reader result_reader;
        private Reader error_reader;

        public ClientListener(ConnectionParameters parameters) : base(parameters) { }

        protected override void bind()
        {
            channel.QueueDeclare(queue: StatusQueue, durable: false, exclusive: false, autoDelete: true, arguments: null);
            channel.QueueDeclare(queue: ResultQueue, durable: true, exclusive: false, autoDelete: false, arguments: null);
            channel.QueueDeclare(queue: ErrorQueue, durable: true, exclusive: false, autoDelete: false, arguments: null);
            channel.BasicConsume(queue: StatusQueue, noAck: true, consumer: status_reader);
            channel.BasicConsume(queue: ResultQueue, noAck: false, consumer: result_reader);
            channel.BasicConsume(queue: ErrorQueue, noAck: false, consumer: error_reader);
        }

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
                    catch (Exception)
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
    }
}
