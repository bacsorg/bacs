#include "bunsan/utility/builder.hpp"

#include "bunsan/filesystem/operations.hpp"
#include "bunsan/tempfile.hpp"

BUNSAN_FACTORY_DEFINE(bunsan::utility::builder)

void bunsan::utility::builder::pack(
    const boost::filesystem::path &src,
    const boost::filesystem::path &bin,
    const boost::filesystem::path &archive,
    const archiver_ptr &archiver_)
{
    // tmp initialization
    filesystem::reset_dir(bin);
    tempfile root_ = tempfile::in_dir(bin);
    tempfile bin_ = tempfile::in_dir(bin);
    root_.auto_remove(false);
    bin_.auto_remove(false);
    filesystem::reset_dir(root_.path());
    filesystem::reset_dir(bin_.path());
    // installation
    install(src, bin_.path(), root_.path());
    // packing
    BOOST_ASSERT_MSG(archiver_, "Archiver pointer should not be null");
    archiver_->pack_contents(archive, root_.path());
}
