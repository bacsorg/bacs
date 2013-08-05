#pragma once

#include <bunsan/pm/entry.hpp>

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

namespace bunsan{namespace pm
{
    struct error: virtual bunsan::error
    {
        error()=default;
        explicit error(const std::string &message_);

        typedef boost::error_info<struct tag_package, entry> package;
        typedef boost::error_info<struct tag_action, std::string> action;
        typedef filesystem::error::path path;
    };

    struct invalid_installation_error: virtual error {};

    struct installation_meta_exists_error: virtual invalid_installation_error
    {
        typedef boost::error_info<struct tag_meta, boost::filesystem::path> meta;
    };

    struct create_error: virtual error {};

    struct source_does_not_exist_error: virtual create_error
    {
        typedef boost::error_info<struct tag_source, std::string> source;
    };

    /// \todo consider rename
    struct invalid_entry_name_error: virtual error
    {
        typedef boost::error_info<struct tag_entry_name, std::string> entry_name;
    };

    struct invalid_configuration_error: virtual error {};

    struct invalid_configuration_key_error: virtual invalid_configuration_error
    {
        typedef boost::error_info<struct tag_configuration_key, std::string> configuration_key;
    };

    struct invalid_configuration_utility_error: virtual invalid_configuration_error
    {
        typedef boost::error_info<struct tag_utility_type, std::string> utility_type;
    };

    struct invalid_configuration_archiver_error: virtual invalid_configuration_utility_error {};
    struct invalid_configuration_source_archiver_error: virtual invalid_configuration_archiver_error {};
    struct invalid_configuration_cache_archiver_error: virtual invalid_configuration_archiver_error {};
    struct invalid_configuration_builder_error: virtual invalid_configuration_utility_error {};
    struct invalid_configuration_fetcher_error: virtual invalid_configuration_utility_error {};

    struct circular_dependencies: virtual error {};
}}
