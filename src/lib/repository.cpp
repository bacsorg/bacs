#include "bacs/archive/repository.hpp"
#include "bacs/archive/config.hpp"
#include "bacs/archive/flags.hpp"
#include "bacs/archive/error.hpp"

#include "bunsan/utility/archiver.hpp"
#include "bunsan/config/cast.hpp"

#include <boost/filesystem/operations.hpp>

using namespace bacs::archive;

repository::repository(const boost::property_tree::ptree &ptree):
    repository(bunsan::config::load<bacs::archive::config>(ptree)) {}

repository::repository(const config &config_):
    m_lock(boost::interprocess::open_or_create, config_.lock.c_str()),
    m_resolver(config_.resolver),
    m_tmpdir(config_.tmpdir),
    m_problem_archiver(config_.problem.archiver.instance(m_resolver)) {}

/* container */

problem::import_map repository::insert_all(const archiver_options &archiver_options_, const boost::filesystem::path &archive)
{
    const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    const bunsan::utility::archiver_ptr archiver = archiver_options_.instance(m_resolver);
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

bunsan::tempfile repository::extract_all(const problem::id_set &id_set, const archiver_options &archiver_options_)
{
    // TODO validate problem id
    const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
    const bunsan::utility::archiver_ptr archiver = archiver_options_.instance(m_resolver);
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
