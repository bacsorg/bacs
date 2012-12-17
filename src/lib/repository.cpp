#include "bacs/archive/repository.hpp"
#include "bacs/archive/config.hpp"
#include "bacs/archive/flags.hpp"
#include "bacs/archive/error.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/utility/archiver.hpp"

using namespace bacs::archive;

namespace
{
    bunsan::utility::archiver_ptr get_archiver(const problem::archiver_config &archiver_config, const bunsan::utility::resolver &m_resolver)
    {
        const bunsan::utility::archiver_ptr archiver = bunsan::utility::archiver::instance(archiver_config.type, m_resolver);
        if (!archiver)
            BOOST_THROW_EXCEPTION(unknown_archiver_error() <<
                                  unknown_archiver_error::archiver_config(archiver_config));
#if 0 // FIXME
        if (archiver_config.format)
            archiver->setarg("format", archiver_config.format.get());
#endif
        return archiver;
    }
}

repository::repository(const boost::property_tree::ptree &config_):
    m_lock(config_.get<std::string>(config::lock)),
    m_resolver(config_.get_child(config::resolver)),
    m_tmpdir(config_.get<std::string>(config::tmpdir))
{
    problem::archiver_config archiver_config = {
        config_.get<std::string>(config::problem::archiver::type),
        config_.get_optional<std::string>(config::problem::archiver::format)
    };
    m_problem_archiver = get_archiver(archiver_config, m_resolver);
}

/* container */

problem::import_map repository::insert_all(const problem::archiver_config &archiver_config, const boost::filesystem::path &archive)
{
    bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    bunsan::utility::archiver_ptr archiver = get_archiver(archiver_config, m_resolver);
    archiver->unpack(archive, unpacked.path());
    problem::import_map map;
    for (boost::filesystem::directory_iterator i(unpacked.path()), end; i != end; ++i)
    {
        problem::id id = i->path().filename().string();
        // TODO validate problem id (should be implemented in repository::insert)
        map[id] = insert(id, i->path());
    }
    return map;
}

bunsan::tempfile repository::extract_all(const problem::id_set &id_set, const problem::archiver_config &archiver_config)
{
    // TODO validate problem id
    bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    bunsan::utility::archiver_ptr archiver = get_archiver(archiver_config, m_resolver);
    for (const problem::id &id: id_set)
    {
        // ignore return value
        extract(id, unpacked.path() / id);
    }
    bunsan::tempfile packed = bunsan::tempfile::in_dir(m_tmpdir);
    archiver->pack_contents(packed.path(), unpacked.path());
    return packed;
}

bool repository::available(const problem::id &id)
{
    return has_flag(id, problem::flag::ignore);
}

/* flags */

problem::id_set repository::ignore_all(const problem::id_set &id_set)
{
    problem::id_set ret;
    for (const auto &id: id_set)
        if (ignore(id))
            ret.insert(id);
    return ret;
}

bool repository::ignore(const problem::id &id)
{
    return set_flag(id, problem::flag::ignore);
}

/* info */

namespace
{
    template <typename Ret, typename RetMap, Ret (repository::*Getter)(const problem::id &)>
    struct multiplex
    {
        template <typename T>
        static inline void insert(RetMap &map, const problem::id &id, const T &ret)
        {
            map[id] = ret;
        }
        template <typename T>
        static inline void insert(RetMap &map, const problem::id &id, const boost::optional<T> &ret)
        {
            if (ret)
                map[id] = ret.get();
        }
        static RetMap get(const problem::id_set &id_set, repository *this_)
        {
            RetMap map;
            for (const problem::id &id: id_set)
                insert(map, id, (this_->*Getter)(id));
            return map;
        }
    };
}

problem::info_map repository::info_all(const problem::id_set &id_set)
{
    return multiplex<boost::optional<problem::info_type>, problem::info_map, &repository::info>::get(id_set, this);
}

problem::hash_map repository::hash_all(const problem::id_set &id_set)
{
    return multiplex<boost::optional<problem::hash_type>, problem::hash_map, &repository::hash>::get(id_set, this);
}

problem::status_map repository::status_all(const problem::id_set &id_set)
{
    return multiplex<problem::status, problem::status_map, &repository::status>::get(id_set, this);
}

/* repack */

problem::import_map repository::repack_all(const problem::id_set &id_set)
{
    return multiplex<problem::import_info, problem::import_map, &repository::repack>::get(id_set, this);
}
