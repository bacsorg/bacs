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

    struct invalid_installation_error: virtual error {};
    struct installation_meta_exists_error: virtual invalid_installation_error
    {
        typedef boost::error_info<struct tag_meta, boost::filesystem::path> meta;
    };

    struct create_error: virtual error {};
    struct source_does_not_exist_error: virtual create_error {};

    struct extract_error: virtual error {};

    struct install_error: virtual error {};

    struct update_error: virtual error {};

    struct need_update_error: virtual error {};

    struct clean_error: virtual error {};

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

    struct circular_dependencies_error: virtual error {};

    // native
    struct native_error: virtual error
    {
        typedef boost::error_info<struct tag_build_dir, boost::filesystem::path> build_dir;
    };

    struct native_update_index_error: virtual native_error {};
    struct native_fetch_index_error: virtual native_error {};
    struct native_fetch_source_error: virtual native_error {};
    struct native_unpack_error: virtual native_error {};
    struct native_pack_error: virtual native_error {};
    struct native_build_empty_error: virtual native_error {};
    struct native_extract_build_error: virtual native_error {};
    struct native_build_installation_error: virtual native_error {};
    struct native_extract_installation_error: virtual native_error
    {
        typedef boost::error_info<struct tag_reset, bool> reset;
    };
    struct native_install_installation_error: virtual native_error {};
    struct native_update_installation_error: virtual native_error {};
    struct native_need_update_installation_error: virtual native_error {};

    // native helpers
    struct native_read_checksum_error: virtual native_error {};
    struct native_write_snapshot_error: virtual native_error {};
    struct native_read_snapshot_error: virtual native_error {};
    struct native_read_index_error: virtual native_error {};
    struct native_check_cache_error: virtual native_error {};
    struct native_check_dir_error: virtual native_error {};
    struct native_build_outdated_error: virtual native_error {};
    struct native_installation_outdated_error: virtual native_error {};
    struct native_check_dirs_error: virtual native_error {};
    struct native_clean_error: virtual native_error {};

    // native create
    struct native_create_error: virtual native_error {};
    struct native_create_recursively_error: virtual native_error {};
}}
