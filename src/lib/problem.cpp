#include "bacs/archive/problem.hpp"

#include "bunsan/pm/entry.hpp"

namespace bacs{namespace archive{namespace problem
{
    bool is_allowed_id(const id &id_)
    {
        return bunsan::pm::entry::is_allowed_subpath(id_);
    }

    bool is_allowed_flag(const flag &flag_)
    {
        return bunsan::pm::entry::is_allowed_subpath(flag_);
    }
}}}
