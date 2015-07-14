#pragma once

#include <bunsan/pm/repository.hpp>

#include <boost/noncopyable.hpp>

namespace bunsan {
namespace pm {

class repository::builder : private boost::noncopyable {
 public:
  builder(repository &self, const build_config &config);

  /// create empty build (for packages without sources)
  void build_empty(const entry &package);
  void build(const entry &package);
  void build_installation(const entry &package);

 private:
  utility::builder_ptr get_builder(const std::string &builder_id);
  utility::builder_ptr get_builder();

  void unpack_source(const entry &package,
                     const boost::filesystem::path &destination,
                     snapshot &snapshot_);

 private:
  cache &cache_();
  distributor &distributor_();
  extractor &extractor_();
  local_system &local_system_();

 private:
  repository &m_self;
  build_config m_config;
  std::unordered_map<std::string, utility::builder_ptr> m_builders;
  utility::builder_ptr m_builder;
};

}  // namespace pm
}  // namespace bunsan
