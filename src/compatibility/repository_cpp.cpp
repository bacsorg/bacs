#include <bunsan/pm/compatibility/repository.hpp>

#include <bunsan/pm/repository.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace bunsan {
namespace pm {
namespace compatibility {

repository::repository(const std::string &config) : m_repo(nullptr) {
  boost::property_tree::ptree cfg;
  boost::property_tree::read_info(config, cfg);
  m_repo = new bunsan::pm::repository(cfg);
}

void repository::extract(const std::string &package,
                         const std::string &destination) {
  m_repo->extract(package, destination);
}

void repository::create(const std::string &path, bool strip) {
  m_repo->create(path, strip);
}

void repository::clean_cache() { m_repo->clean_cache(); }

repository::~repository() { delete m_repo; }

void repository::initialize_cache(const std::string &config) {
  boost::property_tree::ptree cfg;
  boost::property_tree::read_info(config, cfg);
  bunsan::pm::repository::initialize_cache(cfg);
}

}  // namespace compatibility
}  // namespace pm
}  // namespace bunsan
