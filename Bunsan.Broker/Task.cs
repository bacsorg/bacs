using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    public class Task
    {
        [ProtoMember(1, IsRequired = false)]
        public string Worker { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public string Package { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public byte[] Data { get; set; }
    }
}
