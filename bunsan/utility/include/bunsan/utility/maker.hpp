#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

#include <unordered_map>

namespace bunsan::utility {
struct maker_error : virtual error {
  using cwd = boost::error_info<struct tag_cwd, boost::filesystem::path>;
  using targets =
      boost::error_info<struct tag_targets, std::vector<std::string>>;
  using flags = boost::error_info<struct tag_flags,
                                  std::unordered_map<std::string, std::string>>;
};
struct maker_exec_error : virtual maker_error {};

class maker : public utility {
  BUNSAN_FACTORY_BODY(maker, const utility_config &, resolver &)
 public:
  virtual void exec(
      const boost::filesystem::path &cwd,
      const std::vector<std::string> &targets,
      const std::unordered_map<std::string, std::string> &flags) = 0;
};
BUNSAN_FACTORY_TYPES(maker)

}  // namespace bunsan::utility

namespace boost {
std::string to_string(const bunsan::utility::maker_error::targets &targets);
std::string to_string(const bunsan::utility::maker_error::flags &flags);
}  // namespace boost
