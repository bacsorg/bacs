using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker.Rabbit
{
    [ProtoContract]
    class RabbitTask
    {
        [ProtoMember(1, IsRequired = false)]
        public string Identifier { get; set; }
        public Task Task { get; set; }
        public Constraints Constraints { get; set; }
        public string StatusQueue { get; set; }
        public string ResultQueue { get; set; }
    }
}
