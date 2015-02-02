#pragma once

#include <bunsan/protobuf/basic_io.hpp>
#include <bunsan/protobuf/text_parser.hpp>
#include <bunsan/protobuf/text_serializer.hpp>

namespace bunsan{namespace protobuf
{
    using text = basic_io<text_parser, text_serializer>;
}}
