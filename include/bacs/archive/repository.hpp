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
        typedef std::string id;
        typedef std::vector<unsigned char> info;
        typedef bunsan::pm::entry package;
        typedef std::vector<id> id_list;
        typedef std::vector<info> info_list;
        typedef std::vector<package> package_list;
    }
    class repository
    {
    public:
        explicit repository(const boost::property_tree::ptree &config_);
        // container
        problem::id_list insert(/*archive*/);
        problem::id_list erase(const problem::id_list &id_list);
        problem::id_list all();
        // info
        problem::info info(const problem::id &id);
        problem::info_list info(const problem::id_list &id_list);
    private:
    };
}}

#endif //BACS_ARCHIVE_REPOSITORY_HPP

