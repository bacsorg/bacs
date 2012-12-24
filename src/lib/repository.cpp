#include "bacs/archive/repository.hpp"
#include "bacs/archive/config.hpp"
#include "bacs/archive/flags.hpp"
#include "bacs/archive/error.hpp"

#include "bunsan/utility/archiver.hpp"
#include "bunsan/config/cast.hpp"
#include "bunsan/filesystem/operations.hpp"

#include <boost/filesystem/operations.hpp>

namespace bacs{namespace archive
{
    repository::repository(const boost::property_tree::ptree &ptree):
        repository(bunsan::config::load<bacs::archive::config>(ptree)) {}

    repository::repository(const config &config_):
        m_lock(boost::interprocess::open_or_create, config_.lock.c_str()),
        m_resolver(config_.resolver),
        m_tmpdir(config_.tmpdir),
        m_problem_archiver(config_.problem.archiver.instance(m_resolver)) {}

    namespace
    {
        template <typename Ret>
        std::unordered_map<problem::id, Ret> get_all(repository *const this_,
                                                     Ret (repository::*get)(const problem::id &),
                                                     const problem::id_set &id_set)
        {
            std::unordered_map<problem::id, Ret> map;
            for (const problem::id &id: id_set)
                map[id] = (this_->*get)(id);
            return map;
        }

        problem::id_set get_all(repository *const this_,
                                bool (repository::*get)(const problem::id &),
                                const problem::id_set &id_set)
        {
            problem::id_set set;
            for (const problem::id &id: id_set)
                if ((this_->*get)(id))
                    set.insert(id);
            return set;
        }
    }

    /* container */

    problem::import_map repository::insert_all(const boost::filesystem::path &location)
    {
        problem::import_map map;
        for (boost::filesystem::directory_iterator i(location), end; i != end; ++i)
        {
            const problem::id id = i->path().filename().string();
            // TODO validate problem id (should be implemented in repository::insert)
            map[id] = insert(id, i->path());
        }
        return map;
    }

    problem::import_map repository::insert_all(const archiver_options &archiver_options_,
                                               const boost::filesystem::path &archive)
    {
        const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
        const bunsan::utility::archiver_ptr archiver = archiver_options_.instance(m_resolver);
        archiver->unpack(archive, unpacked.path());
        return insert_all(unpacked.path());
    }

    void repository::extract_all(const problem::id_set &id_set,
                                 const boost::filesystem::path &location)
    {
        bunsan::filesystem::reset_dir(location);
        for (const problem::id &id: id_set)
        {
            const boost::filesystem::path dst = location / id;
            if (!extract(id, dst) && boost::filesystem::exists(dst))
                boost::filesystem::remove_all(dst);
        }
    }

    void repository::extract_all(const problem::id_set &id_set,
                                 const archiver_options &archiver_options_,
                                 const boost::filesystem::path &archive)
    {
        const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_tmpdir);
        extract_all(id_set, unpacked.path());
        archiver_options_.instance(m_resolver)->pack_contents(archive, unpacked.path());
    }

    bunsan::tempfile repository::extract_all(const problem::id_set &id_set,
                                             const archiver_options &archiver_options_)
    {
        bunsan::tempfile packed = bunsan::tempfile::in_dir(m_tmpdir);
        extract_all(id_set, archiver_options_, packed.path());
        return packed;
    }

    problem::id_set repository::existing(const problem::id_set &id_set)
    {
        return get_all(this, &repository::exists, id_set);
    }

    problem::id_set repository::available(const problem::id_set &id_set)
    {
        return get_all(this, &repository::is_available, id_set);
    }

    bool repository::is_available(const problem::id &id)
    {
        return has_flag(id, problem::flag::ignore);
    }

    /* flags */

    bool repository::ignore(const problem::id &id)
    {
        return set_flag(id, problem::flag::ignore);
    }

    problem::id_set repository::ignore_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::ignore, id_set);
    }

    problem::id_set repository::clear_flags_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::clear_flags, id_set);
    }

    /* info */

    problem::status_map repository::status_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::status, id_set);
    }

    problem::info_map repository::info_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::info, id_set);
    }

    problem::hash_map repository::hash_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::hash, id_set);
    }

    /* repack */

    problem::import_map repository::repack_all(const problem::id_set &id_set)
    {
        return get_all(this, &repository::repack, id_set);
    }
}}
