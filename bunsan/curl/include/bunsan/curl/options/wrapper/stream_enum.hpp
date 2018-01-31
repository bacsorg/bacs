#pragma once

#include <bunsan/curl/options/traits.hpp>
#include <bunsan/curl/options/wrapper/string.hpp>

#include <curl/curl.h>

#include <boost/lexical_cast.hpp>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <typename Enum>
class stream_enum : public string {
 public:
  explicit stream_enum(const Enum data)
      : string(boost::lexical_cast<std::string>(data)) {}
};

template <typename Enum, Enum Default>
class stream_enum_optional : public stream_enum<Enum> {
 public:
  using stream_enum<Enum>::stream_enum;

  stream_enum_optional() : stream_enum<Enum>(Default) {}
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
