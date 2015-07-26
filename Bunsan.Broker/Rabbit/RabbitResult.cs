using ProtoBuf;

namespace Bunsan.Broker.Rabbit
{
    [ProtoContract]
    class RabbitResult
    {
        [ProtoMember(1, IsRequired = false)]
        public string Identifier { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public Result Result { get; set; }
    }
}
