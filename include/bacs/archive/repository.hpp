#ifndef BACS_ARCHIVE_REPOSITORY_HPP
#define BACS_ARCHIVE_REPOSITORY_HPP

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include "bunsan/tempfile.hpp"
#include "bunsan/pm/entry.hpp"
#include "bunsan/utility/resolver.hpp"

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
        problem::import_map insert_all(const problem::archive_format &format, const boost::filesystem::path &archive);
        /*!
         * \brief extract problems from repository
         *
         * \return archive with problems
         *
         * If problem does not exists for given id, this id is ignored.
         */
        bunsan::tempfile extract_all(const problem::archive_format &format, const problem::id_list &id_list);
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
        problem::import_info insert(const problem::id &problem_id, const boost::filesystem::path &location);
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
         * \return false if problem does not exists
         */
        bool erase(const problem::id &id);
        /*!
         * \brief list of available problems
         */
        problem::id_list all();
        /* info */
        problem::info info(const problem::id &id);
        problem::info_list info(const problem::id_list &id_list);
        problem::hash hash(const problem::id &id);
        problem::hash_list hash(const problem::id_list &id_list);
    private:
        std::string value(const boost::property_tree::ptree::path_type &path);
        const bunsan::utility::resolver m_resolver;
        const boost::filesystem::path m_tmpdir;
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

