#include "bacs/archive/repository.hpp"
#include "bacs/archive/error.hpp"

namespace bacs{namespace archive
{
    problem::import_info repository::repack_(const problem::id &id, const problem::hash &hash)
    {
        problem::import_info import_info;
        // TODO
        import_info.error = "Not implemented";
        return import_info;
    }
}}
