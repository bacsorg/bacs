#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <boost/optional.hpp>

namespace bacs{namespace archive{namespace problem
{
    /// problem id
    typedef std::string id;

    typedef std::vector<unsigned char> binary;

    /// information about problem, see bacs::single::api::problem::Problem
    typedef binary info_type;

    /// hash string
    typedef binary hash_type;

    typedef std::unordered_set<id> id_set;
    typedef std::unordered_map<id, boost::optional<info_type>> info_map;
    typedef std::unordered_map<id, boost::optional<hash_type>> hash_map;
    typedef std::string flag_type;
    typedef std::unordered_set<flag_type> flag_set;

    struct status_type
    {
        flag_set flags;
        hash_type hash;
    };

    typedef std::unordered_map<id, boost::optional<status_type>> status_map;

    struct import_info
    {
        boost::optional<std::string> error;  ///< null if OK, error message otherwise
        boost::optional<status_type> status; ///< null if does not exist
    };

    typedef std::unordered_map<id, import_info> import_map;

    bool is_allowed_id(const id &id_);
}}}
