#pragma once

#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan {
namespace utility {

struct builder_error : virtual error {
  using src = boost::error_info<struct tag_src, boost::filesystem::path>;
  using bin = boost::error_info<struct tag_bin, boost::filesystem::path>;
};
struct builder_install_error : virtual builder_error {
  using root = boost::error_info<struct tag_root, boost::filesystem::path>;
};
struct builder_pack_error : virtual builder_error {
  using archive =
      boost::error_info<struct tag_archive, boost::filesystem::path>;
};

class builder : public utility {
  BUNSAN_FACTORY_BODY(builder, resolver &)
 public:
  /*!
   * \brief Build and install package
   *
   * \param src source files
   * \param bin temporary directory for package building
   * \param root package will be installed into root directory
   */
  virtual void install(const boost::filesystem::path &src,
                       const boost::filesystem::path &bin,
                       const boost::filesystem::path &root) = 0;

  /*!
   * \brief Build and pack package
   *
   * Default implementation will call install
   * with root set to temporary directory inside bin
   * and pack it contents using archiver::pack_contents
   */
  virtual void pack(const boost::filesystem::path &src,
                    const boost::filesystem::path &bin,
                    const boost::filesystem::path &archive,
                    const archiver_ptr &archiver_);
};
BUNSAN_FACTORY_TYPES(builder)

}  // namespace utility
}  // namespace bunsan
