#pragma once

#include <bunsan/error.hpp>
#include <bunsan/system_error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace filesystem {

struct error : virtual bunsan::error {
  using path = boost::error_info<struct tag_path, boost::filesystem::path>;

  using source_path =
      boost::error_info<struct tag_source_path, boost::filesystem::path>;

  using destination_path =
      boost::error_info<struct tag_destination_path, boost::filesystem::path>;

  using openmode =
      boost::error_info<struct tag_openmode, std::ios_base::openmode>;
};

struct system_error : bunsan::system_error, virtual error {
  using bunsan::system_error::system_error;
};

struct read_data_error : virtual error {};
struct read_data_size_limit_exceeded_error : virtual read_data_error {
  using size_limit = boost::error_info<struct tag_size_limit, std::size_t>;
};

struct write_data_error : virtual error {};

}  // namespace filesystem
}  // namespace bunsan

namespace boost {
std::string to_string(const bunsan::filesystem::error::openmode &openmode);
}  // namespace boost
