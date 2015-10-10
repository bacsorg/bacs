#pragma once

#include <bunsan/pm/entry.hpp>

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

namespace bunsan {
namespace pm {

struct error : virtual bunsan::error {
  using package = boost::error_info<struct tag_package, entry>;
  using path = filesystem::error::path;
  using source_id = boost::error_info<struct tag_source_id, std::string>;
  using source = boost::error_info<struct tag_source, boost::filesystem::path>;
  using strip = boost::error_info<struct tag_strip, bool>;
  using destination =
      boost::error_info<struct tag_destination, boost::filesystem::path>;
  using root = boost::error_info<struct tag_root, boost::filesystem::path>;
  using lifetime = boost::error_info<struct tag_lifetime, std::time_t>;
};

struct metadata_error : virtual error {};

struct checksum_error : virtual metadata_error {};
struct read_checksum_error : virtual checksum_error {};
struct write_checksum_error : virtual checksum_error {};

struct snapshot_error : virtual metadata_error {};
struct read_snapshot_error : virtual snapshot_error {};
struct write_snapshot_error : virtual snapshot_error {};

struct invalid_installation_error : virtual error {};
struct installation_meta_exists_error : virtual invalid_installation_error {
  using meta = boost::error_info<struct tag_meta, boost::filesystem::path>;
};

struct repository_error : virtual error {};

struct create_error : virtual repository_error {};
struct source_does_not_exist_error : virtual create_error {};

struct extract_error : virtual repository_error {};

struct install_error : virtual repository_error {};

struct update_error : virtual repository_error {};

struct need_update_error : virtual repository_error {};

struct clean_error : virtual repository_error {};

/// \todo consider rename
struct entry_error : virtual error {};
struct entry_name_error : virtual entry_error {};
struct empty_entry_error : virtual entry_error {};
struct invalid_entry_name_error : virtual entry_name_error {
  using entry_name = boost::error_info<struct tag_entry_name, std::string>;
};

struct invalid_configuration_error : virtual error {};
struct invalid_configuration_key_error : virtual invalid_configuration_error {
  using configuration_key =
      boost::error_info<struct tag_configuration_key, std::string>;
};
struct invalid_configuration_utility_error
    : virtual invalid_configuration_error {
  using utility_type = boost::error_info<struct tag_utility_type, std::string>;
};
struct invalid_configuration_archiver_error
    : virtual invalid_configuration_utility_error {};
struct invalid_configuration_remote_archiver_error
    : virtual invalid_configuration_archiver_error {};
struct invalid_configuration_cache_archiver_error
    : virtual invalid_configuration_archiver_error {};
struct invalid_configuration_builder_error
    : virtual invalid_configuration_utility_error {
  using builder = boost::error_info<struct tag_builder, std::string>;
};
struct invalid_configuration_duplicate_builder_error
    : virtual invalid_configuration_builder_error {};
struct invalid_configuration_builder_not_found_error
    : virtual invalid_configuration_builder_error {};
struct invalid_configuration_fetcher_error
    : virtual invalid_configuration_utility_error {};
struct invalid_configuration_relative_path_error
    : virtual invalid_configuration_error {};

struct circular_dependencies_error : virtual error {};

struct null_local_system_error : virtual invalid_configuration_error {};
struct null_cache_error : virtual invalid_configuration_error {};
struct null_distributor_error : virtual invalid_configuration_error {};
struct null_builder_error : virtual invalid_configuration_error {};
struct null_extractor_error : virtual invalid_configuration_error {};

struct repository_component_error : virtual repository_error {};

// distributor
struct distributor_error : virtual repository_component_error {
  using url = boost::error_info<struct tag_url, std::string>;
};
struct distributor_create_error : virtual distributor_error {};
struct distributor_create_recursively_error : virtual distributor_error {};
struct distributor_update_meta_error : virtual distributor_error {};
struct distributor_update_meta_inconsistent_checksum_error
    : virtual distributor_update_meta_error {};
struct distributor_update_meta_no_package_error
    : virtual distributor_update_meta_error {};
struct distributor_update_sources_error : virtual distributor_error {};

// cache
struct cache_error : virtual repository_component_error {};
struct cache_initialize_error : virtual cache_error {};
struct cache_verify_and_repair_error : virtual cache_error {};
struct cache_verify_and_repair_directory_error : virtual cache_error {};
struct cache_clean_error : virtual cache_error {};
struct cache_read_index_error : virtual cache_error {};
struct cache_read_checksum_error : virtual cache_error {};
struct cache_read_build_snapshot_error : virtual cache_error {};
struct cache_read_installation_snapshot_error : virtual cache_error {};
struct cache_unpack_source_error : virtual cache_error {};
struct cache_unpack_build_error : virtual cache_error {};
struct cache_unpack_installation_error : virtual cache_error {};
struct cache_pack_build_error : virtual cache_error {};
struct cache_pack_installation_error : virtual cache_error {};
struct cache_build_outdated_error : virtual cache_error {};
struct cache_installation_outdated_error : virtual cache_error {};
struct cache_file_path_error : virtual cache_error {
  using root = boost::error_info<struct tag_root, boost::filesystem::path>;
  using filename =
      boost::error_info<struct tag_filename, boost::filesystem::path>;
};
struct cache_file_path_invalid_filename_error : virtual cache_file_path_error {
};

// builder
struct builder_error : virtual repository_component_error {
  using build_dir =
      boost::error_info<struct tag_build_dir, boost::filesystem::path>;
};
struct builder_build_empty_error : virtual builder_error {};
struct builder_build_error : virtual builder_error {};
struct builder_build_installation_error : virtual builder_error {};

// extractor
struct extractor_error : virtual repository_component_error {};
struct extractor_extract_error : virtual extractor_error {};
struct extractor_install_error : virtual extractor_error {};
struct extractor_update_error : virtual extractor_error {};
struct extractor_need_update_error : virtual extractor_error {};
struct extractor_extract_source_error : virtual extractor_error {};
struct extractor_extract_build_error : virtual extractor_error {};
struct extractor_extract_installation_error : virtual extractor_error {};
struct extractor_merge_directories_error : virtual extractor_error {};

}  // namespace pm
}  // namespace bunsan
