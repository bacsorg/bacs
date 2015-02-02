#pragma once

#include <bunsan/protobuf/base_parser.hpp>

namespace bunsan{namespace protobuf
{
    class parser: public base_parser
    {
    protected:
        void merge_raw(
            google::protobuf::Message &message,
            google::protobuf::io::ZeroCopyInputStream &input) override;
    };
}}
