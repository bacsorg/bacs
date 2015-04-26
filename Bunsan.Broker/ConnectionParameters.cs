using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    public class ConnectionParameters
    {
        [ProtoMember(1, IsRequired = false)]
        public string Identifier { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public Credentials Credentials { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public string Host { get; set; }

        [ProtoMember(4, IsRequired = false)]
        public int Port { get; set; }

        [ProtoMember(5, IsRequired = false)]
        public string VirtualHost { get; set; }
    }
}
