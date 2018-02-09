#include <bunsan/pm/cache.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <boost/assert.hpp>
#include <boost/thread/lock_types.hpp>

namespace bunsan::pm {

namespace {
const boost::filesystem::path installation_subdir = ".install";
const boost::filesystem::path installation_data = "data";

pm::config update_config_data(pm::config config) {
  if (!config.extract) BOOST_THROW_EXCEPTION(null_extractor_error());
  config.extract->installation.data = installation_data;
  return config;
}
}  // namespace

cache::cache(const config &config_)
    : m_repository(update_config_data(config_.repository)),
      m_config(config_.cache) {
  clean_();
}

cache::cache(const boost::property_tree::ptree &config_)
    : cache(bunsan::config::load<cache::config>(config_)) {}

cache::entry cache::get(const pm::entry &package) {
  const boost::filesystem::path root =
      m_config.root / package.location() / installation_subdir;
  if (m_repository.need_update(package, root, m_config.lifetime)) {
    const boost::unique_lock<boost::shared_mutex> lk(m_lock);
    const boost::filesystem::file_status status =
        boost::filesystem::symlink_status(root);
    switch (status.type()) {
      default:
        clean_();
      case boost::filesystem::file_not_found:
        boost::filesystem::create_directories(root);
      case boost::filesystem::directory_file:
        m_repository.update(package, root, m_config.lifetime);
    }
  }
  return entry(&m_lock, root / installation_data);
}

void cache::clean() {
  const boost::unique_lock<boost::shared_mutex> lk(m_lock);
  clean_();
}

void cache::clean_() {
  // requires lock
  bunsan::filesystem::reset_dir(m_config.root);
}

const boost::filesystem::path &cache::entry::root() const {
  BOOST_ASSERT(*this);
  return m_root;
}

cache::entry::entry(boost::shared_mutex *lock_,
                    const boost::filesystem::path &root)
    : m_lock(lock_), m_root(root) {
  BOOST_ASSERT(*this);
  lock();
}

void cache::entry::lock() {
  if (*this) m_lock->lock_shared();
}

void cache::entry::unlock() {
  if (m_lock) m_lock->unlock_shared();
  m_lock = nullptr;
}

}  // namespace bunsan::pm
