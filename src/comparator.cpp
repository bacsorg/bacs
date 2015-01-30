#include <bunsan/protobuf/comparator.hpp>

namespace bunsan{namespace protobuf
{
    bool equal(const google::protobuf::Message &a,
               const google::protobuf::Message &b)
    {
        if (a.GetTypeName() != b.GetTypeName())
            return false;
        std::string a_data, b_data;
        if (!a.SerializePartialToString(&a_data))
            return false;
        if (!b.SerializePartialToString(&b_data))
            return false;
        return a_data == b_data;
    }
}}
