#pragma once

#include <bacs/archive/problem.hpp>

#include <bunsan/error.hpp>

namespace bacs {
namespace archive {

struct error : virtual bunsan::error {};

struct protobuf_error : virtual error {};
struct protobuf_parsing_error : virtual protobuf_error {};
struct protobuf_serialization_error : virtual protobuf_error {};

struct problem_error : virtual error {
  using problem_id = boost::error_info<struct tag_problem_id, problem::id>;
};

struct format_error : virtual error {};

struct unknown_archiver_error : virtual error {
  using archiver_type =
      boost::error_info<struct tag_archiver_type, std::string>;
};

namespace problem {
struct error : virtual archive::error {};

struct flag_error : virtual error {
  using flag = boost::error_info<struct tag_flag, problem::flag>;
};
struct invalid_flag_error : virtual flag_error {};
struct flag_cant_be_set_error : virtual flag_error {};

}  // namespace problem

}  // namespace archive
}  // namespace bacs
