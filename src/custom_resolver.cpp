#include <bunsan/utility/custom_resolver.hpp>

#include <bunsan/utility/error.hpp>
#include <bunsan/utility/system_resolver.hpp>

#include <bunsan/config/cast.hpp>

namespace bunsan {
namespace utility {

custom_resolver::custom_resolver()
    : m_back_resolver(std::make_unique<system_resolver>()) {}

custom_resolver::custom_resolver(std::unique_ptr<resolver> back_resolver)
    : m_back_resolver(std::move(back_resolver)) {}

custom_resolver::custom_resolver(const config &config_)
    : custom_resolver(config_, std::make_unique<system_resolver>()) {}

custom_resolver::custom_resolver(const config &config_,
                                 std::unique_ptr<resolver> back_resolver)
    : m_config(config_), m_back_resolver(std::move(back_resolver)) {}

// ptree aliases
custom_resolver::custom_resolver(const boost::property_tree::ptree &ptree)
    : custom_resolver(bunsan::config::load<config>(ptree)) {}

custom_resolver::custom_resolver(const boost::property_tree::ptree &ptree,
                                 std::unique_ptr<resolver> back_resolver)
    : custom_resolver(bunsan::config::load<config>(ptree),
                      std::move(back_resolver)) {}

void custom_resolver::apply_alias(boost::filesystem::path &name) const {
  if (!m_config) return;
  boost::unordered_set<boost::filesystem::path> used;
  used.insert(name);
  auto iter = m_config->alias.find(name);
  // try to resolve as alias while possible
  while (iter != m_config->alias.end()) {
    name = iter->second;
    iter = m_config->alias.find(name);
    if (used.find(name) != used.end())
      BOOST_THROW_EXCEPTION(resolver_circular_alias_dependencies_error());
    used.insert(name);
  }
}

void custom_resolver::apply_absolute(boost::filesystem::path &name) const {
  if (!m_config) return;
  const auto iter = m_config->absolute.find(name);
  if (iter != m_config->absolute.end()) name = iter->second;
}

void custom_resolver::apply_path(boost::filesystem::path &name) const {
  if (!m_config) return;
  if (name != name.filename()) return;
  if (m_config->path.find(name) != m_config->path.end()) apply_back(name);
}

void custom_resolver::apply_back(boost::filesystem::path &name) const {
  BOOST_ASSERT(m_back_resolver);
  name = m_back_resolver->find_executable(name);
}

boost::filesystem::path custom_resolver::find_executable(
    const boost::filesystem::path &exe) {
  try {
    boost::filesystem::path ret = exe;
    apply_alias(ret);
    apply_absolute(ret);
    apply_path(ret);
    return ret;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(resolver_find_executable_error()
                          << resolver_find_executable_error::executable(exe)
                          << enable_nested_current());
  }
}

boost::filesystem::path custom_resolver::find_library(
    const boost::filesystem::path &lib) {
  try {
#warning unimplemented
    return m_back_resolver->find_library(lib);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(resolver_find_library_error()
                          << resolver_find_library_error::library(lib)
                          << enable_nested_current());
  }
}

std::unique_ptr<resolver> custom_resolver::clone() const {
  BOOST_ASSERT(m_back_resolver);
  if (m_config) {
    return std::make_unique<custom_resolver>(*m_config,
                                             m_back_resolver->clone());
  } else {
    return std::make_unique<custom_resolver>(m_back_resolver->clone());
  }
}

}  // namespace utility
}  // namespace bunsan
