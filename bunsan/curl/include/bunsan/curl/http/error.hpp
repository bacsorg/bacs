#pragma once

#include <bunsan/curl/error.hpp>
#include <bunsan/curl/http_version.hpp>

namespace bunsan {
namespace curl {
namespace http {

struct error : virtual curl::error {};

struct header_error : virtual error {
  using name = boost::error_info<struct tag_name, std::string>;
};
struct header_non_unique_value_error : virtual header_error {};
struct header_parse_error : virtual header_error {
  using header_data = boost::error_info<struct tag_header, std::string>;
};
struct header_merge_error : virtual header_error {
  using other_name = boost::error_info<struct tag_other_name, std::string>;
};
struct header_merge_incompatible_name_error : virtual header_merge_error {};

struct status_error : virtual header_error {};
struct status_parse_error : virtual status_error, virtual header_parse_error {};

struct unexpected_header_type_error : virtual header_error {};
struct status_header_expected_error : virtual unexpected_header_type_error {};

struct not_ok_status_error : virtual error {
  using status_http_version =
      boost::error_info<struct tag_status_http_version, curl::http_version>;
  using status_code = boost::error_info<struct tag_status_code, unsigned>;
  using status_reason =
      boost::error_info<struct tag_status_reason, std::string>;
};

}  // namespace http
}  // namespace curl
}  // namespace bunsan
