#ifndef BACS_ARCHIVE_REPOSITORY_HPP
#define BACS_ARCHIVE_REPOSITORY_HPP

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

#include "bunsan/tempfile.hpp"
#include "bunsan/pm/entry.hpp"

namespace bacs{namespace archive
{
    namespace problem
    {
        /// problem id
        typedef std::string id;
        typedef std::vector<unsigned char> binary;
        /// information about problem, see problem.xsd from BACS.XSD
        typedef binary info;
        /// package name
        typedef bunsan::pm::entry package;
        /// hash string
        typedef binary hash;
        typedef std::vector<id> id_list;
        typedef std::vector<info> info_list;
        typedef std::vector<package> package_list;
        typedef std::vector<hash> hash_list;
        struct import_info
        {
            bool ok;
            std::string error;
        };
        typedef std::map<id, import_info> import_map;
    }
    class repository
    {
    public:
        explicit repository(const boost::property_tree::ptree &config_);
        // container
        /*!
         * \brief insert problems into repository
         *
         * \return import information
         */
        problem::import_map insert_all(const std::string &format, const boost::filesystem::path &archive);
        /*
         * \brief extract problems from repository
         *
         * \return archive with problems
         *
         * If problem does not exists for given id, this id is ignored.
         */
        bunsan::tempfile extract(const std::string &format, const problem::id_list &id_list);
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
        problem::id_list erase(const problem::id_list &id_list);
        /*!
         * \brief list of available problems
         */
        problem::id_list all();
        // info
        problem::info info(const problem::id &id);
        problem::info_list info(const problem::id_list &id_list);
        problem::hash hash(const problem::id &id);
        problem::hash_list hash(const problem::id_list &id_list);
    private:
        std::string value(const boost::property_tree::ptree::path_type &path);
        const boost::property_tree::ptree m_config;
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

