using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker.Client
{
    [ProtoContract]
    class ConnectionParameters
    {
        [ProtoMember(1, IsRequired = false)]
        public string identifier { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public string connection { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public Credentials credentials { get; set; }
    }
}
