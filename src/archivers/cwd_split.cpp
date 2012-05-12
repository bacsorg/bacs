#include "cwd_split.hpp"

#include <boost/filesystem/operations.hpp>

void bunsan::utility::archivers::cwd_split::pack(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &file)
{
    const boost::filesystem::path file_ = boost::filesystem::absolute(file);
    const boost::filesystem::path archive_ = boost::filesystem::absolute(archive);
    pack_from(file_.parent_path(), archive_, file_.filename());
}

void bunsan::utility::archivers::cwd_split::pack_content(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &file)
{
    const boost::filesystem::path file_ = boost::filesystem::absolute(file);
    const boost::filesystem::path archive_ = boost::filesystem::absolute(archive);
    pack_from(file_, archive_, ".");
}

