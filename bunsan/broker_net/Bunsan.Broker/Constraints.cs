using ProtoBuf;

namespace Bunsan.Broker
{
    [ProtoContract]
    public class Constraints
    {
        [ProtoMember(1, IsRequired = false)]
        public string[] Resource { get; set; }
    }
}
