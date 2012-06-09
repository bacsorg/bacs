#include "bacs/archive/repository.hpp"
#include "bacs/archive/config.hpp"
#include "bacs/archive/error.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/utility/archiver.hpp"

using namespace bacs::archive;

repository::repository(const boost::property_tree::ptree &config_):
    m_resolver(config_.get_child(config::resolver)),
    m_tmpdir(config_.get<std::string>(config::tmpdir))
{
}

problem::import_map repository::insert_all(const problem::archive_format &format, const boost::filesystem::path &archive)
{
    bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    bunsan::utility::archiver_ptr archiver = bunsan::utility::archiver::instance(format.archiver, m_resolver);
    if (!archiver)
        BOOST_THROW_EXCEPTION(unknown_archiver_error()<<archive_format_error::format(format));
    if (format.format)
        archiver->setarg("format", format.format.get());
    archiver->unpack(archive, unpacked.path());
    boost::filesystem::directory_iterator i(unpacked.path()), end;
    problem::import_map map;
    for (; i!=end; ++i)
    {
        problem::id id = i->path().filename().string();
        map[id] = insert(id, i->path());
    }
    return map;
}

