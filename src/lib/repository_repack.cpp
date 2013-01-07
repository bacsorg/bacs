#include "bacs/archive/repository.hpp"
#include "bacs/archive/error.hpp"
#include "bacs/archive/problem/flags.hpp"

namespace bacs{namespace archive
{
    problem::import_info repository::repack_(const problem::id &id,
                                             const problem::hash &hash,
                                             const boost::filesystem::path &problem_location)
    {
        problem::import_info import_info;
        // TODO
        import_info.error = "Not implemented";
        return import_info;
    }
}}
