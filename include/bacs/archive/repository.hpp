#pragma once

#include "bacs/archive/config.hpp"
#include "bacs/archive/problem.hpp"

#include "bunsan/tempfile.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/archiver.hpp"
#include "bunsan/interprocess/sync/file_guard.hpp"

#include <string>

#include <boost/thread/shared_mutex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

namespace bacs{namespace archive
{
    /*!
     * \brief Problems repository.
     *
     * Member functions has lock-related documentation.
     * User should not rely on it, it may change in the future.
     *
     * \warning User should not create multiple instances bind to single repository
     * (nor in process scope, nor in thread scope). Exception will be thrown.
     *
     * *_all() functions are wrappers for their counterparts
     * without "_all" suffix. These functions return maps, with values
     * returned by functions counterparts.
     *
     * There are three kinds of locks:
     *     - exclusive-lock -- single operation at time, no other locks (except lock-free)
     *     - shared-lock -- multiple operations at time, no exclusive locks
     *     - lock-free -- multiple operations at time
     */
    class repository: private boost::noncopyable
    {
    public:
        explicit repository(const config &config_);
        explicit repository(const boost::property_tree::ptree &tree);

        /* container */
        /*!
         * \brief Insert problems from archive into repository.
         *
         * \return import information
         *
         * Not atomic.
         *
         * \see repository::insert
         */
        problem::import_map insert_all(const boost::filesystem::path &location);

        /*!
         * \copydoc insert_all()
         */
        problem::import_map insert_all(const archiver_options &archiver_options_,
                                       const boost::filesystem::path &archive);

        /*!
         * \brief Insert particular problem into repository.
         *
         * \param location -- directory with problem
         *
         * If conversion is successful, problem flags are reset and data is updated.
         * Otherwise does nothing.
         *
         * Atomic; exclusive-lock on success, lock-free otherwise.
         *
         * \see repository::reset_flags
         */
        problem::import_info insert(const problem::id &id, const boost::filesystem::path &location);

        /// repository::insert(location.filename().string(), location)
        problem::import_info insert(const boost::filesystem::path &location);

        /*!
         * \brief Extract problems from repository.
         *
         * Ignores unavailable problems.
         *
         * Not atomic.
         *
         * \see repository::is_available
         * \see repository::extract
         */
        void extract_all(const problem::id_set &id_set, const boost::filesystem::path &location);

        /*!
         * \copydoc extract_all()
         *
         * \return archive with problems
         */
        void extract_all(const problem::id_set &id_set,
                         const archiver_options &archiver_options_,
                         const boost::filesystem::path &archive);

        /*!
         * \copydoc extract_all()
         */
        bunsan::tempfile extract_all(const problem::id_set &id_set,
                                     const archiver_options &archiver_options_);

        /*!
         * \brief Extract problem from repository.
         *
         * \return false if problem is not available
         *
         * Atomic, shared-lock.
         *
         * \see repository::is_available
         */
        bool extract(const problem::id &id, const boost::filesystem::path &location);

        /*!
         * \brief Change problem id and repack.
         *
         * If problem with id=future exists or repack fails functions does nothing.
         *
         * Atomic, shared-lock, exclusive-lock.
         *
         * \todo full locking documentation
         *
         * \see repository::repack
         */
        problem::import_info rename(const problem::id &current, const problem::id &future);

        /*!
         * \brief Set of existing problems.
         *
         * Not atomic, lock-free.
         *
         * \see repository::exists
         */
        problem::id_set existing();

        /*!
         * \brief Set of available problems.
         *
         * \see repository::is_available
         */
        problem::id_set available();

        /*!
         * \brief Check problem for existence.
         *
         * Albeit problem exists, it may be ignored
         * by some functions.
         *
         * Atomic, lock-free.
         *
         * \see repository::is_available
         */
        bool exists(const problem::id &id);

        /*!
         * \brief Get existing problems from list.
         *
         * Not atomic.
         *
         * \see repository::is_available
         */
        problem::id_set existing(const problem::id_set &id_set);

        /*!
         * \brief Check problem for availability.
         *
         * \return true if problem exists and
         * is not marked by problem::flag::ignore.
         *
         * Atomic, shared-lock.
         *
         * \see repository::exists
         * \see repository::has_flag
         */
        bool is_available(const problem::id &id);

        /*!
         * \brief Get available problems from list.
         *
         * Not atomic.
         *
         * \see repository::is_available
         */
        problem::id_set available(const problem::id_set &id_set);

        /* flags */
        /*!
         * \brief Get problem status.
         *
         * \return null if problem does not exist
         *
         * Atomic, lock-free.
         */
        boost::optional<problem::status_type> status(const problem::id &id);

        /*!
         * \brief Get problems status.
         *
         * Not atomic.
         *
         * \see repository::status
         */
        problem::status_map status_all(const problem::id_set &id_set);

        /*!
         * \brief Check problem for flag.
         *
         * \return false if problem does not exist
         * or problem is not marked by flag
         *
         * Atomic, lock-free.
         */
        bool has_flag(const problem::id &id, const problem::flag &flag);

        /*!
         * \brief Return problems with specified flag.
         *
         * Not atomic.
         *
         * \see repository::has_flag
         */
        problem::id_set with_flag(const problem::id_set &id_set, const problem::flag &flag);

        /*!
         * \brief Set of problems with flag.
         *
         * Not atomic.
         *
         * \see repository::with_flag
         */
        problem::id_set with_flag(const problem::flag &flag);

        /*!
         * \brief Set problem flag.
         *
         * \return false if problem does not exist
         *
         * Atomic, exclusive-lock.
         */
        bool set_flag(const problem::id &id, const problem::flag &flag);

        /*!
         * \brief Set problem flags.
         *
         * Atomic, exclusive-lock.
         */
        bool set_flags(const problem::id &id, const problem::flag_set &flags);

        /*!
         * \brief Set problems flags.
         *
         * Not atomic.
         *
         * \see repository::set_flags
         */
        problem::id_set set_flags_all(const problem::id_set &id_set, const problem::flag_set &flags);

        /*!
         * \brief Unset problem flag.
         *
         * \return false if problem does not exist
         *
         * Atomic, exclusive-lock.
         */
        bool unset_flag(const problem::id &id, const problem::flag &flag);

        /*!
         * \brief Unset problem flags.
         *
         * \return false if problem does not exist
         *
         * Atomic, exclusive-lock.
         */
        bool unset_flags(const problem::id &id, const problem::flag_set &flags);

        /*!
         * \brief Unset problems flags.
         *
         * Not atomic.
         *
         * \see repository::unset_flags
         */
        problem::id_set unset_flags_all(const problem::id_set &id_set, const problem::flag_set &flags);

        /*!
         * \brief Clear problem flags.
         *
         * \return false if problem does not exist
         *
         * Atomic, exclusive-lock.
         */
        bool clear_flags(const problem::id &id);

        /*!
         * \brief Clear problems flags.
         *
         * \return Set of cleared problems.
         *
         * Not atomic.
         *
         * \see repository::clear_flags
         */
        problem::id_set clear_flags_all(const problem::id_set &id_set);

        /*!
         * \brief Mark problems with problem::flag::ignore.
         *
         * If problem does not exists id is ignored.
         *
         * \return set of marked problems
         *
         * Not atomic.
         *
         * \see repository::ignore
         */
        problem::id_set ignore_all(const problem::id_set &id_set);

        /*!
         * \brief Alias for repository::set_flag(id, #problem::flag::ignore).
         *
         * \return false if problem does not exists
         *
         * \see repository::set_flag
         *
         * \todo pair function, function rename possible (as flag)
         */
        bool ignore(const problem::id &id);

        /* info */
        /*!
         * \brief Get problem info.
         *
         * \return uninitialized optional if problem is not available
         *
         * \see repository::is_available
         */
        boost::optional<problem::info_type> info(const problem::id &id);

        /*!
         * \brief Get info map for given problems.
         *
         * If problem is not available, id is ignored.
         *
         * \see repository::is_available
         * \see repository::info
         */
        problem::info_map info_all(const problem::id_set &id_set);

        /*!
         * \brief Get problem hash.
         *
         * \return uninitialized optional if problem is not available
         *
         * \see repository::is_available
         */
        boost::optional<problem::hash_type> hash(const problem::id &id);

        /*!
         * \brief Get hash map for given problems.
         *
         * If problem is not available, id is ignored.
         *
         * \see repository::is_available
         * \see repository::hash
         */
        problem::hash_map hash_all(const problem::id_set &id_set);

        /* repack */
        /*!
         * \brief Repack problem.
         *
         * Repacking is similar to import
         * except using internal copy of problem
         *
         * Can be useful if problem conversion utility is changed
         * or index is corrupted
         *
         * If repack is not successful problem is marked by problem::flag::ignore.
         *
         * Atomic, exclusive-lock.
         *
         * \see repository::set_flag
         */
        problem::import_info repack(const problem::id &id);

        /*!
         * \brief Repack all given problems.
         *
         * Not atomic.
         *
         * \see repository::repack
         */
        problem::import_map repack_all(const problem::id_set &id_set);

    private:
        /*!
         * \brief Check problem for validity.
         *
         * \return true if problem exists and problem index has valid format
         *
         * Check basic format conformance, it does not check
         * whether problem is importable
         *
         * Atomic, shared-lock.
         *
         * \see repository::exists
         */
        bool valid(const problem::id &id);

        /* not synchronized function versions for internal usage */
        bool is_available_(const problem::id &id);

    private:
        boost::shared_mutex m_lock;
        const bunsan::interprocess::file_guard m_flock;
        const bunsan::utility::resolver m_resolver;
        const boost::filesystem::path m_tmpdir;
        /// internal problem storage packing
        const bunsan::utility::archiver_ptr m_problem_archiver;
    };
}}
