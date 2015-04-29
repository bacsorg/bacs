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

        [ProtoMember(2, IsRequired = false)]
        public Task Task { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public Constraints Constraints { get; set; }

        [ProtoMember(4, IsRequired = false)]
        public string StatusQueue { get; set; }

        [ProtoMember(5, IsRequired = false)]
        public string ResultQueue { get; set; }

        [ProtoMember(6, IsRequired = false)]
        public string ErrorQueue { get; set; }
    }
}
