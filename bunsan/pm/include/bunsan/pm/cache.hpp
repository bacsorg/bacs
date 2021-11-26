#pragma once

#include <bunsan/pm/repository.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace bunsan::pm {

class cache : private boost::noncopyable {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar &BOOST_SERIALIZATION_NVP(cache.root);
      ar &BOOST_SERIALIZATION_NVP(cache.lifetime);
      ar &BOOST_SERIALIZATION_NVP(repository);
    }

    struct cache_config {
      boost::filesystem::path root;
      std::time_t lifetime;
    } cache;

    pm::config repository;
  };

 public:
  class entry;

 public:
  explicit cache(const config &config_);
  explicit cache(const boost::property_tree::ptree &config_);

  entry get(const pm::entry &package);

  void clean();

 private:
  void clean_();

 private:
  bunsan::pm::repository m_repository;
  const config::cache_config m_config;
  boost::shared_mutex m_lock;
};

class cache::entry {
 public:
  entry() = default;

  entry(const entry &e) : m_lock(e.m_lock), m_root(e.m_root) { lock(); }

  entry &operator=(const entry &e) {
    entry tmp(e);
    swap(tmp);
    return *this;
  }

  entry(entry &&e) {
    swap(e);
    e.unlock();
  }

  entry &operator=(entry &&e) {
    swap(e);
    e.unlock();
    return *this;
  }

  ~entry() { unlock(); }

  explicit operator bool() const noexcept { return m_lock; }

  const boost::filesystem::path &root() const;

  void swap(entry &e) noexcept {
    using std::swap;

    swap(m_lock, e.m_lock);
    swap(m_root, e.m_root);
  }

 private:
  friend class pm::cache;

  entry(boost::shared_mutex *lock_, const boost::filesystem::path &root);

  void lock();
  void unlock();

 private:
  boost::shared_mutex *m_lock = nullptr;
  boost::filesystem::path m_root;
};

inline void swap(cache::entry &a, cache::entry &b) noexcept { a.swap(b); }

}  // namespace bunsan::pm
