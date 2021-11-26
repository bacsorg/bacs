#include <bunsan/pm/entry.hpp>

#include <bunsan/pm/error.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assert.hpp>

namespace bunsan::pm {

bool entry::is_allowed_symbol(char c) {
  return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') || c == '_' || c == '-' || c == '.';
}

bool entry::is_allowed_subpath(const std::string &subpath) {
  return !subpath.empty() &&
         boost::algorithm::all(subpath, entry::is_allowed_symbol);
}

entry::entry(const std::string &name_, char delim) { build(name_, delim); }

entry::entry(const char *name_, char delim) { build(name_, delim); }

void entry::build(const std::string &name_, char delim) {
  m_location.clear();
  boost::algorithm::split(m_location, name_, [delim](char c) {
    return delim == c;
  }, boost::algorithm::token_compress_on);
  for (const std::string &i : m_location)
    if (!is_allowed_subpath(i))
      BOOST_THROW_EXCEPTION(invalid_entry_name_error()
                            << invalid_entry_name_error::entry_name(name_));
  BOOST_ASSERT(!empty());
}

entry &entry::operator=(entry &&e) noexcept {
  swap(e);
  return *this;
}

entry &entry::operator=(const entry &e) {
  entry ent(e);
  swap(ent);
  return *this;
}

entry &entry::operator=(const std::string &name_) {
  entry ent(name_);
  swap(ent);
  return *this;
}

bool entry::operator==(const entry &e) const {
  return m_location == e.m_location;
}

bool entry::operator<(const entry &e) const {
  return m_location < e.m_location;
}

bool entry::empty() const {
  return m_location.empty();
}

entry entry::operator/(const entry &e) const {
  entry ent(*this);
  ent.m_location.insert(ent.m_location.end(), e.m_location.begin(),
                        e.m_location.end());
  return ent;
}

entry entry::absolute(const entry &root) const {
  entry ent(*this);
  ent.make_absolute(root);
  return ent;
}

void entry::make_absolute(const entry &root) {
  if (empty()) return;
  if (m_location.front() == "." || m_location.front() == "..") {
    m_location.insert(m_location.begin(), root.m_location.begin(),
                      root.m_location.end());
  }
  std::vector<std::string> location;
  for (const std::string &e : m_location) {
    if (e == ".") {
      // nothing
    } else if (e == "..") {
      if (!location.empty()) location.pop_back();
    } else {
      location.push_back(e);
    }
  }
  m_location.swap(location);
}

boost::filesystem::path entry::location() const {
  boost::filesystem::path loc;
  for (const std::string &i : m_location) loc /= i;
  return loc;
}

const std::vector<std::string> &entry::decomposition() const {
  return m_location;
}

boost::property_tree::ptree::path_type entry::ptree_path() const {
  return boost::property_tree::ptree::path_type(name('/'), '/');
}

std::string entry::name(const std::string &delim) const {
  if (empty()) BOOST_THROW_EXCEPTION(empty_entry_error());
  return boost::algorithm::join(m_location, delim);
}

std::string entry::name(char delim) const {
  std::string d = {delim};
  return name(d);
}

void entry::swap(entry &e) noexcept { m_location.swap(e.m_location); }

std::string entry::remote_resource(const std::string &repository,
                                   const boost::filesystem::path &name_) const {
  std::string full = repository;
  if (!full.empty() && full.back() != '/') full.push_back('/');
  full += name('/');
  for (const boost::filesystem::path &i : name_) {
    full.push_back('/');
    full += i.string();
  }
  return full;
}

boost::filesystem::path entry::local_resource(
    const boost::filesystem::path &dir,
    const boost::filesystem::path &name_) const {
  return dir / location() / name_;
}

}  // namespace bunsan::pm
