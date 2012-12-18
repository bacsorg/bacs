#pragma once

#include "bunsan/pm/entry.hpp"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>
#include <map>

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
    typedef std::set<id> id_set;
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
    typedef std::map<id, import_info> import_map;
}}}
