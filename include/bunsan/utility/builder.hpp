#pragma once

#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/error.hpp>
#include <bunsan/utility/resolver.hpp>
#include <bunsan/utility/utility.hpp>

#include <bunsan/factory_helper.hpp>

namespace bunsan {
namespace utility {

struct builder_error : virtual error {
  typedef boost::error_info<struct tag_src, boost::filesystem::path> src;
  typedef boost::error_info<struct tag_bin, boost::filesystem::path> bin;
};
struct builder_install_error : virtual builder_error {
  typedef boost::error_info<struct tag_root, boost::filesystem::path> root;
};
struct builder_pack_error : virtual builder_error {
  typedef boost::error_info<struct tag_archive, boost::filesystem::path>
      archive;
};

class builder : public utility BUNSAN_FACTORY_BEGIN(builder, resolver &) public
                :
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
BUNSAN_FACTORY_END(builder)

}  // namespace utility
}  // namespace bunsan
