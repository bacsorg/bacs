#pragma once

#include <bunsan/config/traits.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>
#include <vector>

namespace bunsan::pm {

/// Represents \ref package_page "package" reference in repository.
class entry {
 public:
  static constexpr char def_delim = '/';

 public:
  // construction
  entry() = default;
  entry(const entry &) = default;
  entry(entry &&) = default;
  entry &operator=(const entry &e);
  entry &operator=(entry &&e) noexcept;
  void swap(entry &e) noexcept;

  // conversions
  entry(const std::string &name_, char delim = def_delim);
  entry(const char *name_, char delim = def_delim);
  entry &operator=(const std::string &name_);

  // comparisons
  bool operator==(const entry &e) const;
  bool operator<(const entry &e) const;

  // operations
  bool empty() const;
  entry operator/(const entry &e) const;
  entry absolute(const entry &root = entry{}) const;
  void make_absolute(const entry &root = entry{});

  // entry conversion getters
  boost::filesystem::path location() const;
  std::string name(char delim = def_delim) const;
  std::string name(const std::string &delim) const;
  const std::vector<std::string> &decomposition() const;
  boost::property_tree::ptree::path_type ptree_path() const;

  /// \returns repository / name('/') / name_
  std::string remote_resource(
      const std::string &repository,
      const boost::filesystem::path &name_ = boost::filesystem::path()) const;

  /// \returns dir / name('/') / name_
  boost::filesystem::path local_resource(
      const boost::filesystem::path &dir,
      const boost::filesystem::path &name_ = boost::filesystem::path()) const;

 public:
  static bool is_allowed_subpath(const std::string &subpath);
  static bool is_allowed_symbol(char c);

 private:
  void build(const std::string &name_, char delim = def_delim);

 private:
  std::vector<std::string> m_location;
};
inline void swap(entry &e1, entry &e2) noexcept { e1.swap(e2); }

template <typename Char, typename Traits>
inline std::basic_istream<Char, Traits> &operator>>(
    std::basic_istream<Char, Traits> &in, entry &e) {
  std::string name;
  in >> name;
  e = name;
  return in;
}

template <typename Char, typename Traits>
inline std::basic_ostream<Char, Traits> &operator<<(
    std::basic_ostream<Char, Traits> &out, const entry &e) {
  return out << e.name();
}

}  // namespace bunsan::pm

namespace bunsan::config::traits {
template <>
struct serializer<bunsan::pm::entry> {
  template <typename Archive>
  static void load(bunsan::pm::entry &obj, Archive &ar, const unsigned int) {
    std::string name_;
    ar >> name_;
    obj = name_;
  }

  template <typename Archive>
  static void save(const bunsan::pm::entry &obj, Archive &ar,
                   const unsigned int) {
    ar << obj.name();
  }
};
}  // namespace bunsan::config::traits
