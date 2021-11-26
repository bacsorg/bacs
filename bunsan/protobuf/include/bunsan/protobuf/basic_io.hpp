#pragma once

#include <google/protobuf/message.h>

#include <utility>

namespace bunsan::protobuf {

template <typename Parser, typename Serializer>
class basic_io {
 public:
  template <typename... Args>
  static void merge_partial(google::protobuf::Message &message,
                            Args &&... args) {
    Parser p;
    p.allow_partial(true);
    p.merge(message, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void merge(google::protobuf::Message &message, Args &&... args) {
    Parser p;
    p.allow_partial(false);
    p.merge(message, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void parse_partial(google::protobuf::Message &message,
                            Args &&... args) {
    Parser p;
    p.allow_partial(true);
    p.parse(message, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void parse(google::protobuf::Message &message, Args &&... args) {
    Parser p;
    p.allow_partial(false);
    p.parse(message, std::forward<Args>(args)...);
  }

  template <typename Message, typename... Args>
  static Message parse_partial_make(Args &&... args) {
    Message message;
    parse_partial(message, std::forward<Args>(args)...);
    return message;
  }

  template <typename Message, typename... Args>
  static Message parse_make(Args &&... args) {
    Message message;
    parse(message, std::forward<Args>(args)...);
    return message;
  }

  template <typename... Args>
  static void serialize_partial(const google::protobuf::Message &message,
                                Args &&... args) {
    Serializer s;
    s.allow_partial(true);
    s.serialize(message, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void serialize(const google::protobuf::Message &message,
                        Args &&... args) {
    Serializer s;
    s.allow_partial(false);
    s.serialize(message, std::forward<Args>(args)...);
  }

  static void append_partial(const google::protobuf::Message &message,
                             std::string &output) {
    Serializer s;
    s.allow_partial(true);
    s.append(message, output);
  }

  static void append(const google::protobuf::Message &message,
                     std::string &output) {
    Serializer s;
    s.allow_partial(false);
    s.append(message, output);
  }

  static void to_string_partial(const google::protobuf::Message &message,
                                std::string &output) {
    Serializer s;
    s.allow_partial(true);
    s.to_string(message, output);
  }

  static void to_string(const google::protobuf::Message &message,
                        std::string &output) {
    Serializer s;
    s.allow_partial(false);
    s.to_string(message, output);
  }

  static std::string to_string_partial(
      const google::protobuf::Message &message) {
    Serializer s;
    s.allow_partial(true);
    return s.to_string(message);
  }

  static std::string to_string(const google::protobuf::Message &message) {
    Serializer s;
    s.allow_partial(false);
    return s.to_string(message);
  }
};

}  // namespace bunsan::protobuf
