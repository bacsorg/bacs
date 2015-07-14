#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan {
namespace utility {
struct maker_error : virtual error {
  using cwd = boost::error_info<struct tag_cwd, boost::filesystem::path>;
  using targets =
      boost::error_info<struct tag_targets, std::vector<std::string>>;
};
struct maker_exec_error : virtual maker_error {};

class maker : public utility BUNSAN_FACTORY_BEGIN(maker, resolver &) public
              : virtual void exec(const boost::filesystem::path &cwd,
                                  const std::vector<std::string> &targets) = 0;
BUNSAN_FACTORY_END(maker)

}  // namespace utility
}  // namespace bunsan

namespace boost {
std::string to_string(const bunsan::utility::maker_error::targets &targets);
}  // namespace boost
