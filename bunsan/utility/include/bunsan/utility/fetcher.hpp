#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan::utility {

struct fetcher_error : virtual error {};
struct fetcher_fetch_error : virtual fetcher_error {
  using uri = boost::error_info<struct tag_uri, std::string>;
  using destination =
      boost::error_info<struct tag_destination, boost::filesystem::path>;
};

class fetcher : public utility {
  BUNSAN_FACTORY_BODY(fetcher, const utility_config &, resolver &)
 public:
  virtual void fetch(const std::string &uri,
                     const boost::filesystem::path &dst) = 0;
};
BUNSAN_FACTORY_TYPES(fetcher)

}  // namespace bunsan::utility
