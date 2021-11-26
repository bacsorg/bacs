#include "cache.hpp"

#include "distributor.hpp"
#include "local_system.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/log/trivial.hpp>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bunsan::pm {

repository::cache::cache(repository &self, const cache_config &config)
    : m_self(self), m_config(config), m_flock(m_config.get_lock()) {
  if (!(m_archiver =
            m_config.archiver.instance_optional(local_system_().resolver()))) {
    BOOST_THROW_EXCEPTION(
        invalid_configuration_cache_archiver_error()
        << invalid_configuration_cache_archiver_error::utility_type(
            m_config.archiver.type));
  }
}

repository::cache::lock_guard repository::cache::lock() {
  return lock_guard(m_flock);
}

void repository::cache::initialize(const cache_config &config) {
  try {
    if (!config.root.is_absolute())
      BOOST_THROW_EXCEPTION(
          invalid_configuration_relative_path_error()
          << invalid_configuration_relative_path_error::path(config.root));
    // ignore if directory exists
    boost::filesystem::create_directory(config.root);
    boost::filesystem::create_directory(config.get_source());
    boost::filesystem::create_directory(config.get_package());

    // lock
    if (!config.get_lock().is_absolute())
      BOOST_THROW_EXCEPTION(invalid_configuration_relative_path_error()
                            << invalid_configuration_relative_path_error::path(
                                config.get_lock()));
    if (boost::filesystem::exists(config.get_lock())) {
      if (!boost::filesystem::is_regular_file(config.get_lock()))
        boost::filesystem::remove(config.get_lock());
    }
    if (!boost::filesystem::exists(config.get_lock())) {
      filesystem::ofstream fout(config.get_lock());
      fout.close();
    }
    initialize_meta(config);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_initialize_error() << enable_nested_current());
  }
}

void repository::cache::initialize_meta(const cache_config &config) {
  cache_config::meta meta;
  meta.version = repository::version();
  boost::property_tree::write_info(
      config.get_meta().string(),
      bunsan::config::save<boost::property_tree::ptree>(meta));
}

void repository::cache::save_meta() { initialize_meta(m_config); }

cache_config::meta repository::cache::load_meta() {
  boost::property_tree::ptree pmeta;
  boost::property_tree::read_info(m_config.get_meta().string(), pmeta);
  return bunsan::config::load<cache_config::meta>(pmeta);
}

void repository::cache::verify_and_repair() {
  try {
    bool outdated = false;
    try {
      const auto meta = load_meta();
      if (meta.version != repository::version()) {
        BUNSAN_LOG_INFO << "Cache's version \"" << meta.version
                        << "\" is not equal to repository's version \""
                        << repository::version() << "\", resetting cache";
        outdated = true;
      }
    } catch (std::exception &) {
      BUNSAN_LOG_ERROR << "Unable to read cache's meta, resetting cache";
      outdated = true;
    }
    if (outdated) {
      clean_();
      save_meta();
    }
    verify_and_repair_directory(m_config.get_source());
    verify_and_repair_directory(m_config.get_package());
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_verify_and_repair_error()
                          << enable_nested_current());
  }
}

void repository::cache::verify_and_repair_directory(
    const boost::filesystem::path &path) {
  try {
    if (!path.is_absolute())
      BOOST_THROW_EXCEPTION(
          invalid_configuration_relative_path_error()
          << invalid_configuration_relative_path_error::path(path));
    BUNSAN_LOG_TRACE << "Checking " << path;
    if (!boost::filesystem::is_directory(path)) {
      if (!boost::filesystem::exists(path)) {
        BUNSAN_LOG_ERROR << "Directory " << path << " was not found";
      } else {
        BUNSAN_LOG_ERROR << path
                         << " is not a directory: starting recursive remove";
        boost::filesystem::remove_all(path);
      }
      if (boost::filesystem::create_directory(path))
        BUNSAN_LOG_TRACE << "Created missing " << path << " directory";
    }
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_verify_and_repair_directory_error()
                          << cache_verify_and_repair_directory_error::path(path)
                          << enable_nested_current());
  }
}

void repository::cache::clean_() {
  try {
    filesystem::reset_dir(m_config.get_source());
    filesystem::reset_dir(m_config.get_package());
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_clean_error() << enable_nested_current());
  }
}

void repository::cache::clean() {
  try {
    verify_and_repair();
    clean_();
  } catch (cache_clean_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_clean_error() << enable_nested_current());
  }
}

index repository::cache::read_index(const entry &package) {
  try {
    return index(index_path(package)).absolute(package);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_read_index_error()
                          << cache_read_index_error::package(package)
                          << enable_nested_current());
  }
}

snapshot_entry repository::cache::read_checksum(const entry &package) {
  try {
    return pm::read_checksum(checksum_path(package));
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_read_checksum_error()
                          << cache_read_checksum_error::package(package)
                          << enable_nested_current());
  }
}

snapshot repository::cache::read_build_snapshot(const entry &package) {
  try {
    return read_snapshot(build_snapshot_path(package));
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_read_build_snapshot_error()
                          << cache_read_build_snapshot_error::package(package)
                          << enable_nested_current());
  }
}

snapshot repository::cache::read_installation_snapshot(const entry &package) {
  try {
    return read_snapshot(installation_snapshot_path(package));
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_read_installation_snapshot_error()
                          << cache_read_installation_snapshot_error::package(
                                 package) << enable_nested_current());
  }
}

void repository::cache::unpack_source(
    const entry &package, const std::string &source_id,
    const boost::filesystem::path &destination) {
  try {
    distributor_().archiver().unpack(source_path(package, source_id),
                                     destination);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_unpack_source_error()
                          << cache_unpack_source_error::package(package)
                          << cache_unpack_source_error::destination(destination)
                          << enable_nested_current());
  }
}

void repository::cache::unpack_build(
    const entry &package, const boost::filesystem::path &destination) {
  try {
    m_archiver->unpack(build_archive_path(package), destination);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_unpack_build_error()
                          << cache_unpack_build_error::package(package)
                          << cache_unpack_build_error::destination(destination)
                          << enable_nested_current());
  }
}

void repository::cache::unpack_installation(
    const entry &package, const boost::filesystem::path &destination) {
  try {
    m_archiver->unpack(installation_archive_path(package), destination);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_unpack_installation_error()
                          << cache_unpack_installation_error::package(package)
                          << cache_unpack_installation_error::destination(
                                 destination) << enable_nested_current());
  }
}

void repository::cache::pack_build(const entry &package,
                                   const boost::filesystem::path &path) {
  try {
    m_archiver->pack_contents(build_archive_path(package), path);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_pack_build_error()
                          << cache_pack_build_error::package(package)
                          << cache_pack_build_error::path(path)
                          << enable_nested_current());
  }
}

void repository::cache::pack_installation(const entry &package,
                                          const boost::filesystem::path &path) {
  try {
    m_archiver->pack_contents(installation_archive_path(package), path);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_pack_installation_error()
                          << cache_pack_installation_error::package(package)
                          << cache_pack_installation_error::path(path)
                          << enable_nested_current());
  }
}

bool repository::cache::build_outdated(const entry &package,
                                       const snapshot &snapshot_) {
  try {
    const boost::filesystem::path snp = build_snapshot_path(package);
    const boost::filesystem::path build = build_archive_path(package);
    if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
      return true;
    return snapshot_ != read_snapshot(snp);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_build_outdated_error()
                          << cache_build_outdated_error::package(package)
                          << enable_nested_current());
  }
}

bool repository::cache::installation_outdated(const entry &package,
                                              const snapshot &snapshot_) {
  try {
    const boost::filesystem::path snp = installation_snapshot_path(package);
    const boost::filesystem::path installation =
        installation_archive_path(package);
    if (!boost::filesystem::exists(snp) ||
        !boost::filesystem::exists(installation))
      return true;
    return snapshot_ != read_snapshot(snp);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_installation_outdated_error()
                          << cache_installation_outdated_error::package(package)
                          << enable_nested_current());
  }
}

boost::filesystem::path repository::cache::checksum_path(const entry &package) {
  return source_file_path(package, format().name.get_checksum());
}

boost::filesystem::path repository::cache::index_path(const entry &package) {
  return source_file_path(package, format().name.get_index());
}

boost::filesystem::path repository::cache::build_archive_path(
    const entry &package) {
  return package_file_path(package, m_config.name.get_build_archive());
}

boost::filesystem::path repository::cache::build_snapshot_path(
    const entry &package) {
  return package_file_path(package, m_config.name.get_build_snapshot());
}

boost::filesystem::path repository::cache::installation_archive_path(
    const entry &package) {
  return package_file_path(package, m_config.name.get_installation_archive());
}

boost::filesystem::path repository::cache::installation_snapshot_path(
    const entry &package) {
  return package_file_path(package, m_config.name.get_installation_snapshot());
}

boost::filesystem::path repository::cache::source_path(
    const entry &package, const std::string &source_id) {
  return source_file_path(package, source_id + format().name.suffix.archive);
}

boost::filesystem::path repository::cache::file_path(
    const boost::filesystem::path &root, const entry &package,
    const boost::filesystem::path &filename) {
  try {
    if (filename.filename() != filename)
      BOOST_THROW_EXCEPTION(
          cache_file_path_invalid_filename_error()
          << cache_file_path_invalid_filename_error::filename(filename));
    const boost::filesystem::path package_root = root / package.location();
    boost::filesystem::create_directories(package_root);
    return package_root / filename;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cache_file_path_error()
                          << cache_file_path_error::root(root)
                          << cache_file_path_error::package(package)
                          << cache_file_path_error::filename(filename)
                          << enable_nested_current());
  }
}

boost::filesystem::path repository::cache::source_file_path(
    const entry &package, const boost::filesystem::path &filename) {
  return file_path(m_config.get_source(), package, filename);
}

boost::filesystem::path repository::cache::package_file_path(
    const entry &package, const boost::filesystem::path &filename) {
  return file_path(m_config.get_package(), package, filename);
}

const format_config &repository::cache::format() {
  return distributor_().format();
}

repository::local_system &repository::cache::local_system_() {
  return m_self.local_system_();
}

repository::distributor &repository::cache::distributor_() {
  return m_self.distributor_();
}

}  // namespace bunsan::pm
