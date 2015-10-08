#include <bunsan/config.hpp>

#include "builder.hpp"

#include "cache.hpp"
#include "distributor.hpp"
#include "extractor.hpp"
#include "merge_maps.hpp"
#include "local_system.hpp"

#include <bunsan/log/trivial.hpp>

#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace pm {

repository::builder::builder(repository &self, const build_config &config)
    : m_self(self), m_config(config) {
  for (const auto builder_cfg : config.builders) {
    if (m_builders.find(builder_cfg.first) != m_builders.end())
      BOOST_THROW_EXCEPTION(
          invalid_configuration_duplicate_builder_error()
          << invalid_configuration_duplicate_builder_error::builder(
              builder_cfg.first));
    const auto builder =
        builder_cfg.second.instance_optional(local_system_().resolver());
    if (!builder)
      BOOST_THROW_EXCEPTION(
          invalid_configuration_builder_error()
          << invalid_configuration_builder_error::builder(builder_cfg.first)
          << invalid_configuration_builder_error::utility_type(
                 builder_cfg.second.type));
    m_builders[builder_cfg.first] = builder;
  }
  m_builder = get_builder(distributor_().format().builder);
}

void repository::builder::build_empty(const entry &package) {
  try {
    BUNSAN_LOG_DEBUG << "Starting \"" << package << "\" " << __func__;
    const tempfile build_dir = local_system_().tempdir_for_build();
    // create empty archive
    cache_().pack_build(package, build_dir.path());
    const snapshot snapshot_ = {{package, cache_().read_checksum(package)}};
    write_snapshot(cache_().build_snapshot_path(package), snapshot_);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(builder_build_empty_error()
                          << builder_build_empty_error::package(package)
                          << enable_nested_current());
  }
}

void repository::builder::build(const entry &package) {
  try {
    BUNSAN_LOG_DEBUG << "Starting \"" << package << "\" " << __func__;
    const tempfile build_dir_ = local_system_().tempdir_for_build();
    const boost::filesystem::path build_dir = build_dir_.path();
    const boost::filesystem::path src =
        build_dir / m_config.name.dir.get_source();
    const boost::filesystem::path build =
        build_dir / m_config.name.dir.get_build();
    const boost::filesystem::path installation =
        build_dir / m_config.name.dir.get_installation();
    // create/clean directories
    boost::filesystem::create_directory(src);
    boost::filesystem::create_directory(build);
    boost::filesystem::create_directory(installation);
    // unpack source
    snapshot snapshot_;
    unpack_source(package, src, snapshot_);
    get_builder()->install(build_dir / m_config.name.dir.get_source(),
                           build_dir / m_config.name.dir.get_build(),
                           build_dir / m_config.name.dir.get_installation());
    cache_().pack_build(package,
                        build_dir / m_config.name.dir.get_installation());
    write_snapshot(cache_().build_snapshot_path(package), snapshot_);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(builder_build_error()
                          << builder_build_error::package(package)
                          << enable_nested_current());
  }
}

void repository::builder::unpack_source(
    const entry &package, const boost::filesystem::path &destination,
    snapshot &snapshot_) {
  BUNSAN_LOG_DEBUG << "Starting " << package << " import unpack";
  const index deps = cache_().read_index(package);
  // extract sources
  for (const auto &i : deps.source.self)
    extractor_().extract_source(package, i.source, destination / i.path);
  // dump package checksum into snapshot
  {
    const auto iter = snapshot_.find(package);
    const snapshot_entry checksum = cache_().read_checksum(package);
    if (iter != snapshot_.end())
      BOOST_ASSERT(iter->second == checksum);
    else
      snapshot_[package.name()] = checksum;
  }
  // extract source imports
  for (const auto &i : deps.source.import.source)
    unpack_source(i.package, destination / i.path, snapshot_);
  // extract package imports
  for (const auto &i : deps.source.import.package) {
    extractor_().extract_installation(i.package, destination / i.path);
    merge_maps(snapshot_, cache_().read_installation_snapshot(i.package));
  }
}

void repository::builder::build_installation(const entry &package) {
  try {
    BUNSAN_LOG_DEBUG << "Starting \"" << package << "\" " << __func__;
    const tempfile build_dir = local_system_().tempdir_for_build();
    boost::filesystem::path install_dir =
        build_dir.path() / m_config.name.dir.get_installation();
    // unpack
    extractor_().extract_build(package, install_dir);
    snapshot snapshot_ = cache_().read_build_snapshot(package);
    const index deps = cache_().read_index(package);
    for (const auto &i : deps.package.self)
      extractor_().extract_source(package, i.source, install_dir / i.path);
    for (const auto &i : deps.package.import.source)
      unpack_source(i.package, install_dir / i.path, snapshot_);
    for (const auto &i : deps.package.import.package) {
      extractor_().extract_installation(i.package, install_dir / i.path);
      merge_maps(snapshot_, cache_().read_installation_snapshot(i.package));
    }
    // save
    cache_().pack_installation(package, install_dir);
    write_snapshot(cache_().installation_snapshot_path(package), snapshot_);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(builder_build_installation_error()
                          << builder_build_installation_error::package(package)
                          << enable_nested_current());
  }
}

bunsan::utility::builder_ptr repository::builder::get_builder(
    const std::string &builder_id) {
  const auto builder_iter = m_builders.find(builder_id);
  if (builder_iter == m_builders.end())
    BOOST_THROW_EXCEPTION(
        invalid_configuration_builder_not_found_error()
        << invalid_configuration_builder_not_found_error::builder(builder_id));
  return builder_iter->second;
}

bunsan::utility::builder_ptr repository::builder::get_builder() {
  return m_builder;
}

repository::cache &repository::builder::cache_() { return m_self.cache_(); }

repository::distributor &repository::builder::distributor_() {
  return m_self.distributor_();
}

repository::extractor &repository::builder::extractor_() {
  return m_self.extractor_();
}

repository::local_system &repository::builder::local_system_() {
  return m_self.local_system_();
}

}  // namespace pm
}  // namespace bunsan
