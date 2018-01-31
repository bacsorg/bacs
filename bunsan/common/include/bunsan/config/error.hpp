#pragma once

#include <bunsan/error.hpp>

namespace bunsan {
namespace config {

struct error : virtual bunsan::error {};

struct input_archive_error : virtual error {};

struct output_archive_error : virtual error {};

struct variant_load_error : virtual input_archive_error {};

struct variant_load_key_error : virtual variant_load_error {
  using key = boost::error_info<struct tag_key, std::string>;
};

struct variant_load_invalid_key_error : virtual variant_load_key_error {};

struct variant_load_no_key_error : virtual variant_load_key_error {};

struct variant_load_multiple_keys_error : virtual variant_load_key_error {};

}  // namespace config
}  // namespace bunsan
