#pragma once

#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/builder.hpp>
#include <bunsan/utility/factory_options.hpp>
#include <bunsan/utility/fetcher.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>

namespace bunsan::pm {

struct format_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(builder);
    ar & BOOST_SERIALIZATION_NVP(archiver);
    ar & BOOST_SERIALIZATION_NVP(name);
  }

  std::string builder;
  utility::factory_options<utility::archiver> archiver;

  struct {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(suffix.archive);
      ar & BOOST_SERIALIZATION_NVP(index);
      ar & BOOST_SERIALIZATION_NVP(checksum);
    }

    struct {
      std::string archive;
    } suffix;

    boost::optional<std::string> index;
    boost::optional<std::string> checksum;

    std::string get_index() const { return index.get_value_or("index"); }
    std::string get_checksum() const { return index.get_value_or("checksum"); }
  } name;
};

struct remote_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(url);
    ar & BOOST_SERIALIZATION_NVP(fetcher);
    ar & BOOST_SERIALIZATION_NVP(format);
  }

  std::string url;
  utility::factory_options<utility::fetcher> fetcher;
  format_config format;
};

struct build_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(builders);
    ar & BOOST_SERIALIZATION_NVP(name);
  }

  std::unordered_map<std::string, utility::factory_options<utility::builder>>
      builders;

  struct {
    template <typename Archive>
    void serialize(Archive &, const unsigned int) {
      // nop
    }

    struct {
      boost::filesystem::path get_source() const { return "source"; }
      boost::filesystem::path get_build() const { return "build"; }
      boost::filesystem::path get_installation() const {
        return "installation";
      }
    } dir;
  } name;
};

struct local_system_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(resolver);
    ar & BOOST_SERIALIZATION_NVP(build_dir);
    ar & BOOST_SERIALIZATION_NVP(tmp_file);
  }

  boost::property_tree::ptree resolver;
  boost::filesystem::path build_dir;  ///< directory for large temporary files
  /// file mask for small temporary files,
  /// should be absolute, '%' occurrences
  /// are replaced by random symbols
  /// by default is placed into system tempdir
  boost::optional<boost::filesystem::path> tmp_file;
};

struct cache_config {
  struct meta {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(version);
    }

    std::string version;
  };

  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(root);
    ar & BOOST_SERIALIZATION_NVP(lock);
    ar & BOOST_SERIALIZATION_NVP(archiver);
    ar & BOOST_SERIALIZATION_NVP(name);
  }

  boost::filesystem::path root;

  /// lock file for synchronization,
  /// placed somewhere inside root
  /// if not specified
  /// \warning filesystem must support flock(2)
  boost::optional<boost::filesystem::path> lock;

  boost::filesystem::path get_lock() const {
    return lock.get_value_or(root / "lock");
  }
  boost::filesystem::path get_meta() const { return root / "meta"; }
  boost::filesystem::path get_source() const { return root / "source"; }
  boost::filesystem::path get_package() const { return root / "package"; }

  utility::factory_options<utility::archiver> archiver;

  struct {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(suffix.archive);
    }

    struct {
      std::string archive;
    } suffix;

    boost::filesystem::path get_build_archive() const {
      return "build" + suffix.archive;
    }
    boost::filesystem::path get_build_snapshot() const {
      return "build_snapshot";
    }

    boost::filesystem::path get_installation_archive() const {
      return "installation" + suffix.archive;
    }
    boost::filesystem::path get_installation_snapshot() const {
      return "installation_snapshot";
    }
  } name;
};

struct extract_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(installation);
  }

  struct {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(meta);
      ar & BOOST_SERIALIZATION_NVP(data);
    }

    boost::filesystem::path meta;
    /// if null data is placed in root folder
    boost::optional<boost::filesystem::path> data;
  } installation;
};

struct config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(remote);
    ar & BOOST_SERIALIZATION_NVP(build);
    ar & BOOST_SERIALIZATION_NVP(local_system);
    ar & BOOST_SERIALIZATION_NVP(cache);
    ar & BOOST_SERIALIZATION_NVP(extract);
  }

  boost::optional<remote_config> remote;
  boost::optional<build_config> build;
  boost::optional<local_system_config> local_system;
  boost::optional<cache_config> cache;
  boost::optional<extract_config> extract;
};

}  // namespace bunsan::pm
