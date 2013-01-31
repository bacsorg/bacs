#pragma once

#include <string>
#include <vector>

namespace bacs{namespace problem
{
    /// problem id
    typedef std::string id;

    typedef std::vector<unsigned char> binary;

    /// information about problem
    struct info
    {
        std::string type; ///< generator project name
        binary data;      ///< generated data
    };

    /// hash string
    typedef binary hash;

    bool is_allowed_id(const id &id_);

    /// \throws invalid_id_error if !is_allowed_id(id_)
    void validate_id(const id &id_);
}}
