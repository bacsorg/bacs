#pragma once

#include <bunsan/pm/entry.hpp>

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

namespace bunsan{namespace pm
{
    struct error: virtual bunsan::error
    {
        typedef boost::error_info<struct tag_package, entry> package;
        typedef filesystem::error::path path;
        typedef boost::error_info<struct tag_source, boost::filesystem::path> source;
        typedef boost::error_info<struct tag_strip, bool> strip;
        typedef boost::error_info<struct tag_destination, boost::filesystem::path> destination;
        typedef boost::error_info<struct tag_root, boost::filesystem::path> root;
        typedef boost::error_info<struct tag_lifetime, std::time_t> lifetime;
    };

    struct metadata_error: virtual error {};

    struct checksum_error: virtual metadata_error {};
    struct read_checksum_error: virtual checksum_error {};
    struct write_checksum_error: virtual checksum_error {};

    struct snapshot_error: virtual metadata_error {};
    struct read_snapshot_error: virtual snapshot_error {};
    struct write_snapshot_error: virtual snapshot_error {};

    struct invalid_installation_error: virtual error {};
    struct installation_meta_exists_error: virtual invalid_installation_error
    {
        typedef boost::error_info<struct tag_meta, boost::filesystem::path> meta;
    };

    struct repository_error: virtual error {};

    struct create_error: virtual repository_error {};
    struct source_does_not_exist_error: virtual create_error {};

    struct extract_error: virtual repository_error {};

    struct install_error: virtual repository_error {};

    struct update_error: virtual repository_error {};

    struct need_update_error: virtual repository_error {};

    struct clean_error: virtual repository_error {};

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
    struct invalid_configuration_remote_archiver_error: virtual invalid_configuration_archiver_error {};
    struct invalid_configuration_cache_archiver_error: virtual invalid_configuration_archiver_error {};
    struct invalid_configuration_builder_error: virtual invalid_configuration_utility_error
    {
        typedef boost::error_info<struct tag_builder, std::string> builder;
    };
    struct invalid_configuration_duplicate_builder_error: virtual invalid_configuration_builder_error {};
    struct invalid_configuration_builder_not_found_error: virtual invalid_configuration_builder_error {};
    struct invalid_configuration_fetcher_error: virtual invalid_configuration_utility_error {};

    struct circular_dependencies_error: virtual error {};

    struct null_local_system_error: virtual invalid_configuration_error {};
    struct null_cache_error: virtual invalid_configuration_error {};
    struct null_distributor_error: virtual invalid_configuration_error {};
    struct null_builder_error: virtual invalid_configuration_error {};
    struct null_extractor_error: virtual invalid_configuration_error {};

    struct repository_component_error: virtual repository_error {};

    // distributor
    struct distributor_error: virtual repository_component_error
    {
        typedef boost::error_info<struct tag_url, std::string> url;
    };
    struct distributor_create_error: virtual distributor_error {};
    struct distributor_create_recursively_error: virtual distributor_error {};
    struct distributor_update_meta_error: virtual distributor_error {};
    struct distributor_update_meta_inconsistent_checksum_error:
        virtual distributor_update_meta_error {};
    struct distributor_update_meta_no_package_error: virtual distributor_update_meta_error {};
    struct distributor_update_sources_error: virtual distributor_error {};
}}
