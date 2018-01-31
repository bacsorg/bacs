#pragma once

#include <bunsan/utility/error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include <memory>

namespace bunsan {
namespace utility {

struct resolver_error : virtual error {};
struct resolver_find_error : virtual resolver_error {};
struct resolver_find_executable_error : virtual resolver_find_error {
  using executable =
      boost::error_info<struct tag_executable, boost::filesystem::path>;
};
struct resolver_find_library_error : virtual resolver_find_error {
  using library =
      boost::error_info<struct tag_library, boost::filesystem::path>;
};
struct resolver_circular_alias_dependencies_error : virtual resolver_error {};

class resolver : private boost::noncopyable {
 public:
  virtual ~resolver();

  virtual boost::filesystem::path find_executable(
      const boost::filesystem::path &exe) = 0;

  virtual boost::filesystem::path find_library(
      const boost::filesystem::path &lib) = 0;

  virtual std::unique_ptr<resolver> clone() const = 0;
};

}  // namespace utility
}  // namespace bunsan
