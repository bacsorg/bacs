#include "cwd_split.hpp"

#include <boost/filesystem/operations.hpp>

namespace bunsan::utility::archivers {

void cwd_split::pack(const boost::filesystem::path &archive,
                     const boost::filesystem::path &file) {
  try {
    const boost::filesystem::path file_ = boost::filesystem::absolute(file);
    const boost::filesystem::path archive_ =
        boost::filesystem::absolute(archive);
    pack_from(file_.parent_path(), archive_, file_.filename());
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(archiver_pack_error()
                          << archiver_pack_error::archive(archive)
                          << archiver_pack_error::file(file)
                          << enable_nested_current());
  }
}

void cwd_split::pack_contents(const boost::filesystem::path &archive,
                              const boost::filesystem::path &file) {
  try {
    const boost::filesystem::path file_ = boost::filesystem::absolute(file);
    const boost::filesystem::path archive_ =
        boost::filesystem::absolute(archive);
    pack_from(file_, archive_, ".");
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(archiver_pack_contents_error()
                          << archiver_pack_contents_error::archive(archive)
                          << archiver_pack_contents_error::file(file)
                          << enable_nested_current());
  }
}

}  // namespace bunsan::utility::archivers
