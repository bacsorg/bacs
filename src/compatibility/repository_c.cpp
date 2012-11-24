#include "bunsan/pm/compatibility/repository.h"
#include "bunsan/pm/repository.hpp"

#include "bunsan/property_tree/info_parser.hpp"

#include <cstring>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using bunsan::pm::repository;

namespace
{
    template <typename ... MemFnArgs, typename ... Args>
    int wrap(void (repository::*mem_fn)(MemFnArgs...), cstring config, string error_msg, size_type error_size, Args &&...args) noexcept
    {
        try
        {
            boost::property_tree::ptree cfg;
            bunsan::property_tree::read_info(config, cfg);
            bunsan::pm::repository repo(cfg);
            (repo.*mem_fn)(std::forward<Args>(args)...);
            return 0;
        }
        catch (std::exception &e)
        {
            std::strncpy(error_msg, e.what(), error_size);
            return 1;
        }
        catch (...)
        {
            std::strncpy(error_msg, "[unknown]", error_size);
            return -1;
        }
    }
}

int bunsan_pm_repository_create(cstring config, cstring source, bool strip, string error_msg, size_type error_size)
{
    return ::wrap(&repository::create, config, error_msg, error_size, source, strip);
}

int bunsan_pm_repository_clean(cstring config, string error_msg, size_type error_size)
{
    return ::wrap(&repository::clean, config, error_msg, error_size);
}

int bunsan_pm_repository_extract(cstring config, cstring package, cstring destination, string error_msg, size_type error_size)
{
    return ::wrap(&repository::extract, config, error_msg, error_size, package, destination);
}
