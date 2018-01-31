#pragma once

#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <map>
#include <set>
#include <unordered_set>

namespace bunsan {
namespace pm {

struct tree_import {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(path);
    ar & BOOST_SERIALIZATION_NVP(package);
  }

  bool operator==(const tree_import &o) const {
    return path == o.path && package == o.package;
  }

  boost::filesystem::path path;
  entry package;
};

struct local_import {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(path);
    ar & BOOST_SERIALIZATION_NVP(source);
  }

  bool operator==(const local_import &o) const {
    return path == o.path && source == o.source;
  }

  boost::filesystem::path path;
  std::string source;
};

using tree_import_list = std::vector<tree_import>;
using local_import_list = std::vector<local_import>;

struct package_stage {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(self);
    ar & BOOST_SERIALIZATION_NVP(import.source);
    ar & BOOST_SERIALIZATION_NVP(import.package);
  }

  struct {
    tree_import_list source, package;
  } import;
  local_import_list self;

  bool empty() const;

  std::set<entry> all() const;
};

struct index {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(source);
    ar & BOOST_SERIALIZATION_NVP(package);
  }

  index() = default;
  index(const index &) = default;
  index(index &&) = default;
  index &operator=(const index &) = default;
  index &operator=(index &&) = default;

  template <typename T>
  explicit index(const T &obj) {
    load(obj);
  }

  explicit operator boost::property_tree::ptree() const;

  void load(const boost::property_tree::ptree &ptree);
  void load(const boost::filesystem::path &path);
  void save(const boost::filesystem::path &path) const;

  index absolute(const entry &root) const;
  void make_absolute(const entry &root);

  // TODO make it unordered
  std::set<entry> all() const;

  std::unordered_set<std::string> sources() const;

  package_stage source, package;
};

}  // namespace pm
}  // namespace bunsan
