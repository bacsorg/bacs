using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    class Connection
    {
        [ProtoMember(1, IsRequired = false)]
        public string Host { get; set; }
        public int Port { get; set; }
        public string VirtualHost { get; set; }
    }
}
