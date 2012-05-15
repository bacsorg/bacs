#ifndef BACS_ARCHIVE_REPOSITORY_HPP
#define BACS_ARCHIVE_REPOSITORY_HPP

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

#include "bunsan/pm/entry.hpp"

namespace bacs{namespace archive
{
    namespace problem
    {
        /// problem id
        typedef std::string id;
        /// information about problem, see problem.xsd from BACS.XSD
        typedef std::vector<unsigned char> info;
        /// package name
        typedef bunsan::pm::entry package;
        /// hash string
        typedef std::vector<unsigned char> hash;
        typedef std::vector<id> id_list;
        typedef std::vector<info> info_list;
        typedef std::vector<package> package_list;
        typedef std::vector<hash> hash_list;
    }
    class repository
    {
    public:
        explicit repository(const boost::property_tree::ptree &config_);
        // container
        /*!
         * \brief insert problems into repository
         *
         * \return list of inserted problem ids
         */
        problem::id_list insert(/*archive*/);
        /*!
         * \brief erase problems from repository
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
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

