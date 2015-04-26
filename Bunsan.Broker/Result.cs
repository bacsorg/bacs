using ProtoBuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bunsan.Broker
{
    [ProtoContract]
    public enum ResultStatus
    {
        [ProtoEnum]
        OK = 0,
        [ProtoEnum]
        INVALID_PROTO = 1,
        [ProtoEnum]
        UNKNOWN_TYPE = 2,
        [ProtoEnum]
        PACKAGE_NOT_FOUND = 3,
        [ProtoEnum]
        PACKAGE_BUILD_ERROR = 4,
        [ProtoEnum]
        INVALID_PACKAGE = 5,
        [ProtoEnum]
        EXECUTION_ERROR = 6,
    }

    [ProtoContract]
    public class Result
    {
        [ProtoMember(1, IsRequired = false)]
        public ResultStatus Status { get; set; }

        [ProtoMember(2, IsRequired = false)]
        public string Reason { get; set; }

        [ProtoMember(3, IsRequired = false)]
        public byte[] Data { get; set; }
    }
}
