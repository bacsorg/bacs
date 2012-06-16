#ifndef BACS_ARCHIVE_PROBLEM_HPP
#define BACS_ARCHIVE_PROBLEM_HPP

#include <string>
#include <vector>
#include <map>

#include <boost/optional.hpp>

#include "bunsan/pm/entry.hpp"

namespace bacs{namespace archive{namespace problem
{
    /// problem id
    typedef std::string id;
    typedef std::vector<unsigned char> binary;
    /// information about problem, see problem.xsd from BACS.XSD
    typedef binary info_type;
    /// package name
    typedef bunsan::pm::entry package;
    /// hash string
    typedef binary hash_type;
    typedef std::vector<id> id_set;
    typedef std::map<id, info_type> info_map;
    typedef std::map<id, hash_type> hash_map;
    typedef std::string flag_type;
    typedef std::set<flag_type> flag_set;
    struct status
    {
        bool exists;
        flag_set flags;
        hash_type hash;
    };
    typedef std::map<id, status> status_map;
    struct import_info
    {
        bool ok;
        std::string error;
        status previous, current;
    };
    struct archiver_config
    {
        std::string type;
        boost::optional<std::string> format;
    };
    typedef std::map<id, import_info> import_map;
}}}

#endif //BACS_ARCHIVE_PROBLEM_HPP

