using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    class Constraints
    {
        [ProtoMember(1, IsRequired = false)]
        public string[] Resource { get; set; }
    }
}
