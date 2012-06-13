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

namespace
{
    bunsan::utility::archiver_ptr get_archiver(const problem::archive_format &format, const bunsan::utility::resolver &m_resolver)
    {
        bunsan::utility::archiver_ptr archiver = bunsan::utility::archiver::instance(format.archiver, m_resolver);
        if (!archiver)
            BOOST_THROW_EXCEPTION(unknown_archiver_error()<<unknown_archiver_error::format(format));
        if (format.format)
            archiver->setarg("format", format.format.get());
        return archiver;
    }
}

problem::import_map repository::insert_all(const problem::archive_format &format, const boost::filesystem::path &archive)
{
    // TODO validate problem id
    bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    bunsan::utility::archiver_ptr archiver = get_archiver(format, m_resolver);
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

bunsan::tempfile repository::extract_all(const problem::archive_format &format, const problem::id_list &id_list)
{
    // TODO validate problem id
    bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    bunsan::utility::archiver_ptr archiver = get_archiver(format, m_resolver);
    for (const problem::id &id: id_list)
    {
        // ignore return value
        extract(id, unpacked.path()/id);
    }
    bunsan::tempfile packed = bunsan::tempfile::in_dir(m_tmpdir);
    archiver->pack_contents(packed.path(), unpacked.path());
    return packed;
}

namespace
{
    template <typename Ret, typename RetMap, typename boost::optional<Ret> (repository::*Getter)(const problem::id &)>
    struct multiplex
    {
        static RetMap get(const problem::id_list &id_list, repository *this_)
        {
            RetMap map;
            for (const problem::id &id: id_list)
            {
                const boost::optional<Ret> ret = (this_->*Getter)(id);
                if (ret)
                    map[id] = ret.get();
            }
            return map;
        }
    };
}

problem::info_map repository::info_all(const problem::id_list &id_list)
{
    return multiplex<problem::info, problem::info_map, &repository::info>::get(id_list, this);
}

problem::hash_map repository::hash_all(const problem::id_list &id_list)
{
    return multiplex<problem::hash, problem::hash_map, &repository::hash>::get(id_list, this);
}

