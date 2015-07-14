#include <bunsan/utility/builder.hpp>

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/tempfile.hpp>

BUNSAN_FACTORY_DEFINE(bunsan::utility::builder)

namespace bunsan {
namespace utility {

void builder::pack(const boost::filesystem::path &src,
                   const boost::filesystem::path &bin,
                   const boost::filesystem::path &archive,
                   const archiver_ptr &archiver_) {
  // prepare empty directories
  filesystem::reset_dir(bin);
  tempfile root_ = tempfile::directory_in_directory(bin);
  tempfile bin_ = tempfile::directory_in_directory(bin);
  root_.auto_remove(false);
  bin_.auto_remove(false);
  // installation
  install(src, bin_.path(), root_.path());
  // packing
  BOOST_ASSERT_MSG(archiver_, "Archiver pointer should not be null");
  archiver_->pack_contents(archive, root_.path());
}

}  // namespace utility
}  // namespace bunsan
