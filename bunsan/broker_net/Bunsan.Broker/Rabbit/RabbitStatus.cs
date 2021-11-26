using ProtoBuf;

namespace Bunsan.Broker.Rabbit
{
    [ProtoContract]
    class RabbitStatus
    {
        [ProtoMember(1, IsRequired = false)]
        public string Identifier { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public Status Status { get; set; }
    }
}
