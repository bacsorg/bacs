using ProtoBuf;

namespace Bunsan.Broker
{
    [ProtoContract]
    public class Credentials
    {
        [ProtoMember(1, IsRequired = false)]
        public string Method { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public string UserName { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public string Password { get; set; }

        [ProtoMember(4, IsRequired = false)]
        public byte[] Certificate { get; set; }

        [ProtoMember(5, IsRequired = false)]
        public byte[] Key { get; set; }
    }
}
