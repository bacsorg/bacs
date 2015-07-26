using System;
using System.IO;
using Bunsan.Broker.Rabbit;
using ProtoBuf;
using RabbitMQ.Client.Exceptions;

namespace Bunsan.Broker
{
    public class ClientSender : ClientBase
    {
        public ClientSender(ConnectionParameters parameters) : base(parameters) { }

        public void Send(Constraints constraints, string id, Task task)
        {
            var rabbit_task = new RabbitTask
            {
                Identifier = id,
                Task = task,
                Constraints = constraints,
                ResultQueue = ResultQueue,
                StatusQueue = StatusQueue,
            };
            byte[] data;
            using (var stream = new MemoryStream())
            {
                Serializer.Serialize(stream, rabbit_task);
                data = new byte[stream.Length];
                stream.Seek(0, SeekOrigin.Begin);
                stream.Read(data, 0, data.Length);
            }
            if (constraints.Resource.Length != 1)
                throw new NotImplementedException("Supports only single resource constraint");
            for (; ; )
            {
                try
                {
                    var properties = connection.Channel.CreateBasicProperties();
                    properties.ReplyTo = ErrorQueue;
                    properties.CorrelationId = id;
                    properties.DeliveryMode = 2;  // persistent
                    if (connection.IsClosed) throw new InvalidOperationException("Already closed");
                    connection.Channel.BasicPublish(exchange: "",
                                                    routingKey: constraints.Resource[0],
                                                    basicProperties: properties,
                                                    body: data);
                    break;
                }
                catch (AlreadyClosedException)
                {
                    // retry
                }
            }
        }
    }
}
