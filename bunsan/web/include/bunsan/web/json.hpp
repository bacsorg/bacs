#pragma once

#include <bunsan/web/error.hpp>

#include <cppcms/json.h>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace web {
namespace json {

struct error : virtual web::error {
  using line_number = boost::error_info<struct tag_line_number, int>;
};
struct load_file_error : virtual error {};
struct save_file_error : virtual error {};

cppcms::json::value load_file(const boost::filesystem::path &path);

void save_file(const cppcms::json::value &value,
               const boost::filesystem::path &path,
               int how = cppcms::json::compact);
}

}  // namespace web
}  // namespace bunsan
