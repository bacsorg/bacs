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
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    public class ClientSender : ClientBase
    {
        public ClientSender(ConnectionParameters parameters) : base(parameters) { }

        protected override void bind() { }

        public void Send(Constraints constraints, string id, Task task)
        {
            reconnect();
            RabbitTask rabbit_task = new RabbitTask()
            {
                Identifier = id,
                Task = task,
                Constraints = constraints,
                ResultQueue = ResultQueue,
                StatusQueue = StatusQueue,
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
            properties.ReplyTo = ErrorQueue;
            properties.CorrelationId = id;
            properties.DeliveryMode = 2;  // persistent
            for (; ; )
            {
                try
                {
                    lock (Lock)
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
    }
}
