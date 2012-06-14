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
    class repository
    {
    public:
        explicit repository(const boost::property_tree::ptree &config_);
        /* container */
        /*!
         * \brief insert problems into repository
         *
         * \return import information
         */
        problem::import_map insert_all(const problem::archiver_config &archiver_config, const boost::filesystem::path &archive);
        /*!
         * \brief extract problems from repository
         *
         * \return archive with problems
         *
         * If problem does not exists for given id, this id is ignored.
         */
        bunsan::tempfile extract_all(const problem::archiver_config &archiver_config, const problem::id_list &id_list);
        /*!
         * \brief extract problem from repository
         *
         * \return false if problem does not exist
         */
        bool extract(const problem::id &id, const boost::filesystem::path &dst);
        /*!
         * \brief insert particular problem into repository
         *
         * \param location -- directory with problem
         */
        problem::import_info insert(const problem::id &id, const boost::filesystem::path &location);
        /*!
         * \brief erase problems from repository
         *
         * If problem does not exists id is ignored.
         *
         * \return list of erased problems
         */
        problem::id_list erase_all(const problem::id_list &id_list);
        /*!
         * \brief erase problem from repository
         *
         * \todo Should problem be moved into backup storage?
         * Or it simply mark it 'erased'? \see repository::repack
         *
         * \return false if problem does not exists
         */
        bool erase(const problem::id &id);
        /*!
         * \brief list of available problems
         */
        problem::id_list all();
        /* info */
        /*!
         * \brief get problem info
         *
         * \return uninitialized optional if problem does not exist
         */
        boost::optional<problem::info> info(const problem::id &id);
        /*!
         * \brief get info map for given problems
         *
         * If problem does not exist id is ignored.
         */
        problem::info_map info_all(const problem::id_list &id_list);
        /*!
         * \brief get problem hash
         *
         * \return unitialized optional if problem does not exist
         */
        boost::optional<problem::hash> hash(const problem::id &id);
        /*!
         * \brief get hash map for given problems
         *
         * If problem does not exist id is ignored.
         */
        problem::hash_map hash_all(const problem::id_list &id_list);
        /*!
         * \brief check problem existence
         *
         * Operation is atomic, lock-free
         *
         * \todo consistency: if 'erased' is a mark
         * then repository::exists should have at least two variants (for mark and for real existence)
         * \see repository::valid
         */
        bool exists(const problem::id &id);
        /*!
         * \brief repack problem
         *
         * Repacking is similar to import
         * except using internal copy of problem
         *
         * Can be useful if problem conversion utility is changed
         * or index is corrupted
         *
         * \todo inconsistent API: if repack is failed, problem is still present.
         * It may be useful to mark problem 'erased', depending on repository::erase
         * implementation.
         */
        problem::import_info repack(const problem::id &id);
        /*!
         * \brief repack all given problems
         */
        problem::import_map repack_all(const problem::id_list &id_list);
    private:
        /*!
         * \brief Check problem for validity
         *
         * Check basic format conformance, it does not check
         * whether problem is importable
         */
        bool valid(const problem::id &id);

        bunsan::interprocess::file_lock m_lock;
        const bunsan::utility::resolver m_resolver;
        const boost::filesystem::path m_tmpdir;
        /// internal problem storage packing
        bunsan::utility::archiver_ptr m_problem_archiver;
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

