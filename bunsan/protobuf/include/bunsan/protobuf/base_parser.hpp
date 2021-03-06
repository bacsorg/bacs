#pragma once

#include <bunsan/protobuf/base_io.hpp>
#include <bunsan/protobuf/error.hpp>
#include <bunsan/protobuf/initialization.hpp>

#include <google/protobuf/message.h>

#include <boost/filesystem/path.hpp>

#include <utility>

namespace bunsan::protobuf {

class base_parser : public base_io {
 public:
  template <typename... Args>
  void merge(google::protobuf::Message &message, Args &&... args) {
    try {
      merge_raw(message, std::forward<Args>(args)...);
    } catch (std::exception &) {
      BOOST_THROW_EXCEPTION(parse_error() << enable_nested_current());
    }
    if (!allow_partial() && !message.IsInitialized()) {
      BOOST_THROW_EXCEPTION(
          fill_initialization_error(parse_initialization_error(), message));
    }
  }

  template <typename... Args>
  void parse(google::protobuf::Message &message, Args &&... args) {
    message.Clear();
    merge(message, std::forward<Args>(args)...);
  }

  template <typename Message, typename... Args>
  Message parse_make(Args &&... args) {
    Message message;
    parse(message, std::forward<Args>(args)...);
    return message;
  }

 protected:
  virtual void merge_raw(google::protobuf::Message &message,
                         google::protobuf::io::ZeroCopyInputStream &input) = 0;

  virtual void merge_raw(google::protobuf::Message &message,
                         google::protobuf::io::ZeroCopyInputStream &input,
                         std::size_t size);

  virtual void merge_raw(google::protobuf::Message &message, const void *data,
                         std::size_t size);

  virtual void merge_raw(google::protobuf::Message &message,
                         const std::string &input);

  virtual void merge_raw(google::protobuf::Message &message,
                         std::istream &input);

  virtual void merge_raw(google::protobuf::Message &message,
                         const boost::filesystem::path &path);
};

}  // namespace bunsan::protobuf
