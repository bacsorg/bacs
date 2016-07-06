#pragma once

#include <boost/noncopyable.hpp>

#include <string>

#include <ctime>

namespace bunsan {
namespace pm {

class repository;

namespace compatibility {
class repository : private boost::noncopyable {
 public:
  /// \param config path to configuration file
  explicit repository(const std::string &config);
  void create(const std::string &path, bool strip = false);
  void create_recursively(const std::string &root, bool strip = false);
  void extract(const std::string &package, const std::string &path);
  void install(const std::string &package, const std::string &destination);
  void update(const std::string &package, const std::string &destination);
  void update(const std::string &package, const std::string &destination,
              const std::time_t &lifetime);
  bool need_update(const std::string &package, const std::string &destination,
                   const std::time_t &lifetime);
  void clean_cache();
  ~repository();

 public:
  static void initialize_cache(const std::string &config);

 private:
  bunsan::pm::repository *m_repo;
};
}  // namespace compatibility

}  // namespace pm
}  // namespace bunsan
