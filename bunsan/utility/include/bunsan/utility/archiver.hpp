#pragma once

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan::utility {

struct archiver_error : virtual error {
  using archive =
      boost::error_info<struct tag_archive, boost::filesystem::path>;
  using file = boost::error_info<struct tag_file, boost::filesystem::path>;
};
struct archiver_pack_error : virtual archiver_error {};
struct archiver_pack_contents_error : virtual archiver_error {};
struct archiver_unpack_error : virtual archiver_error {};

class archiver : public utility {
  BUNSAN_FACTORY_BODY(archiver, const utility_config &, resolver &)
 public:
  virtual void pack(const boost::filesystem::path &archive,
                    const boost::filesystem::path &file) = 0;

  virtual void pack_contents(const boost::filesystem::path &archive,
                             const boost::filesystem::path &dir) = 0;

  virtual void unpack(const boost::filesystem::path &archive,
                      const boost::filesystem::path &dir) = 0;
};
BUNSAN_FACTORY_TYPES(archiver)

}  // namespace bunsan::utility
