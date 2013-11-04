#include <bacs/problem/id.hpp>

#include <bunsan/pm/entry.hpp>

namespace bacs{namespace problem
{
    bool is_allowed_id(const id &id_)
    {
        return bunsan::pm::entry::is_allowed_subpath(id_);
    }

    void validate_id(const id &id_)
    {
        if (!is_allowed_id(id_))
            BOOST_THROW_EXCEPTION(invalid_id_error() <<
                                  invalid_id_error::id(id_));
    }
}}
