#include <bacs/archive/repository.hpp>

#include <bacs/archive/config.hpp>
#include <bacs/archive/error.hpp>
#include <bacs/archive/problem/flags.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/get.hpp>
#include <bunsan/system_error.hpp>
#include <bunsan/utility/archiver.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <utility>

#include <cstdio>

namespace bacs{namespace archive
{
    repository::repository(const boost::property_tree::ptree &ptree):
        repository(bunsan::config::load<bacs::archive::config>(ptree)) {}

    repository::repository(const config &config_):
        m_flock(config_.lock),
        m_resolver(config_.resolver),
        m_location(config_.location),
        m_problem_archiver_factory(config_.problem.data.archiver.configured_factory()),
        m_problem(config_.problem),
        m_importer(config_.problem.importer),
        m_repository(config_.pm) {}

    namespace
    {
        template <typename Ret, typename ... Args>
        std::unordered_map<problem::id, Ret> get_all_map(
            repository *const this_, Ret (repository::*get)(const problem::id &, Args...),
            const problem::id_set &id_set, Args &&...args)
        {
            std::unordered_map<problem::id, Ret> map;
            for (const problem::id &id: id_set)
                map[id] = (this_->*get)(id, std::forward<Args>(args)...);
            return map;
        }

        template <typename ... Args>
        problem::id_set get_all_set(
            repository *const this_, bool (repository::*get)(const problem::id &, Args...),
            const problem::id_set &id_set, Args &&...args)
        {
            problem::id_set set;
            for (const problem::id &id: id_set)
                if ((this_->*get)(id, std::forward<Args>(args)...))
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
        const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_location.tmpdir);
        BOOST_VERIFY(boost::filesystem::create_directory(unpacked.path()));
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
        const bunsan::tempfile unpacked = bunsan::tempfile::in_dir(m_location.tmpdir);
        extract_all(id_set, unpacked.path());
        archiver_options_.instance(m_resolver)->pack_contents(archive, unpacked.path());
    }

    bunsan::tempfile repository::extract_all(const problem::id_set &id_set,
                                             const archiver_options &archiver_options_)
    {
        bunsan::tempfile packed = bunsan::tempfile::in_dir(m_location.tmpdir);
        extract_all(id_set, archiver_options_, packed.path());
        return packed;
    }

    problem::id_set repository::existing(const problem::id_set &id_set)
    {
        return get_all_set(this, &repository::exists, id_set);
    }

    problem::id_set repository::available(const problem::id_set &id_set)
    {
        return get_all_set(this, &repository::is_available, id_set);
    }

    bool repository::is_locked(const problem::id &id)
    {
        return has_flag(id, problem::flags::locked) || has_flag(id, problem::flags::read_only);
    }

    bool repository::is_read_only(const problem::id &id)
    {
        return has_flag(id, problem::flags::read_only);
    }
    /* flags */

    problem::id_set repository::with_flag(const problem::id_set &id_set,
                                          const problem::flag &flag)
    {
        return get_all_set(this, &repository::has_flag, id_set, flag);
    }

    problem::id_set repository::with_flag(const problem::flag &flag)
    {
        return get_all_set(this, &repository::has_flag, existing(), flag);
    }

    problem::id_set repository::set_flags_all(const problem::id_set &id_set,
                                              const problem::flag_set &flags)
    {
        return get_all_set(this, &repository::set_flags, id_set, flags);
    }

    problem::id_set repository::unset_flags_all(const problem::id_set &id_set,
                                                const problem::flag_set &flags)
    {
        return get_all_set(this, &repository::unset_flags, id_set, flags);
    }

    bool repository::ignore(const problem::id &id)
    {
        return set_flag(id, problem::flags::ignore);
    }

    problem::id_set repository::ignore_all(const problem::id_set &id_set)
    {
        return get_all_set(this, &repository::ignore, id_set);
    }

    problem::id_set repository::clear_flags_all(const problem::id_set &id_set)
    {
        return get_all_set(this, &repository::clear_flags, id_set);
    }

    /* info */

    problem::status_map repository::status_all(const problem::id_set &id_set)
    {
        return get_all_map(this, &repository::status, id_set);
    }

    problem::info_map repository::info_all(const problem::id_set &id_set)
    {
        return get_all_map(this, &repository::info, id_set);
    }

    problem::hash_map repository::hash_all(const problem::id_set &id_set)
    {
        return get_all_map(this, &repository::hash, id_set);
    }

    /* repack */

    problem::import_map repository::repack_all(const problem::id_set &id_set)
    {
        return get_all_map(this, &repository::repack, id_set);
    }
}}
