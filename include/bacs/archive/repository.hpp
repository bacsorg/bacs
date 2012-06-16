#ifndef BACS_ARCHIVE_REPOSITORY_HPP
#define BACS_ARCHIVE_REPOSITORY_HPP

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include "bunsan/tempfile.hpp"
#include "bunsan/interprocess/sync/file_lock.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/archiver.hpp"

#include "bacs/archive/problem.hpp"

namespace bacs{namespace archive
{
    /*!
     * \brief Problems repository.
     *
     * Member functions has lock-related documentation.
     * User should not rely on it, it may change in the future.
     *
     * There are three kinds of locks:
     *     - exclusive-lock -- single operation at time, no other locks (except lock-free)
     *     - shared-lock -- multiple operations at time, no exclusive locks
     *     - lock-free -- multiple operations at time
     */
    class repository
    {
    public:
        explicit repository(const boost::property_tree::ptree &config_);
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
        problem::import_map insert_all(const problem::archiver_config &archiver_config, const boost::filesystem::path &archive);
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
        /*!
         * \brief Extract problems from repository.
         *
         * \return archive with problems
         *
         * Ignores unavailable problems.
         *
         * Not atomic.
         *
         * \see repository::available
         * \see repository::extract
         */
        bunsan::tempfile extract_all(const problem::id_set &id_set, const problem::archiver_config &archiver_config);
        /*!
         * \brief Extract problem from repository.
         *
         * \return false if problem is not available
         *
         * Atomic, shared-lock.
         *
         * \see repository::available
         */
        bool extract(const problem::id &id, const boost::filesystem::path &dst);
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
         * \brief Set of available problems.
         *
         * \see repository::available
         */
        problem::id_set all();
        /*!
         * \brief Check problem for existence.
         *
         * Albeit problem exists, it may be ignored
         * by some functions.
         *
         * Atomic, lock-free.
         *
         * \see repository::available
         */
        bool exists(const problem::id &id);
        /*!
         * \brief Check problem for availability.
         *
         * Return true if problem is available and
         * is not marked by 'ignore' flag.
         *
         * Atomic, shared-lock.
         *
         * \see repository::exists
         * \see repository::has_flag
         */
        bool available(const problem::id &id);
        /* flags */
        /*!
         * \brief Get problem status.
         *
         * Atomic, shared-lock.
         */
        problem::status status(const problem::id &id);
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
         * Atomic, lock-free.
         */
        bool has_flag(const problem::id &id, const problem::flag_type &flag);
        /*!
         * \brief Set problem flag.
         *
         * Atomic, exclusive-lock.
         */
        bool set_flag(const problem::id &id, const problem::flag_type &flag);
        /*!
         * \brief Set problem flags.
         *
         * Atomic, exclusive-lock.
         */
        bool set_flags(const problem::id &id, const problem::flag_set &flags);
        /*!
         * \brief Unset problem flag.
         *
         * Atomic, exclusive-lock.
         */
        bool unset_flag(const problem::id &id, const problem::flag_type &flag);
        /*!
         * \brief Unset problem flags.
         *
         * Atomic, exclusive-lock.
         */
        bool unset_flags(const problem::id &id, const problem::flag_set &flags);
        /*!
         * \brief Reset problem flags.
         *
         * Atomic, exclusive-lock.
         */
        bool reset_flags(const problem::id &id);
        /*!
         * \brief Mark problems with 'ignore' flag.
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
         * \brief Alias for set_flag(id, "ignore").
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
         * \see repository::available
         */
        boost::optional<problem::info_type> info(const problem::id &id);
        /*!
         * \brief Get info map for given problems.
         *
         * If problem is not available, id is ignored.
         *
         * \see repository::available
         * \see repository::info
         */
        problem::info_map info_all(const problem::id_set &id_set);
        /*!
         * \brief Get problem hash.
         *
         * \return uninitialized optional if problem is not available
         *
         * \see repository::available
         */
        boost::optional<problem::hash_type> hash(const problem::id &id);
        /*!
         * \brief Get hash map for given problems.
         *
         * If problem is not available, id is ignored.
         *
         * \see repository::available
         * \see repository::hash
         */
        problem::hash_map hash_all(const problem::id_set &id_set);
        /*!
         * \brief Repack problem.
         *
         * Repacking is similar to import
         * except using internal copy of problem
         *
         * Can be useful if problem conversion utility is changed
         * or index is corrupted
         *
         * If repack is not successful problem is marked by 'ignore' flag.
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
        /* lock-free function versions for internal usage */
        // TODO

        bunsan::interprocess::file_lock m_lock;
        const bunsan::utility::resolver m_resolver;
        const boost::filesystem::path m_tmpdir;
        /// internal problem storage packing
        bunsan::utility::archiver_ptr m_problem_archiver;
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

