using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    class Credentials
    {
        [ProtoMember(1, IsRequired = false)]
        public string method { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public string username { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public string password { get; set; }

        [ProtoMember(4, IsRequired = false)]
        public byte[] certificate { get; set; }

        [ProtoMember(5, IsRequired = false)]
        public byte[] key { get; set; }
    }
}
