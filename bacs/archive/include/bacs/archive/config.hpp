#pragma once

#include <bacs/archive/archiver_options.hpp>

#include <bunsan/pm/config.hpp>
#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs {
namespace archive {

struct location_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(tmpdir);
    ar & BOOST_SERIALIZATION_NVP(repository_root);
    ar & BOOST_SERIALIZATION_NVP(pm_repository_root);
  }

  boost::filesystem::path tmpdir;
  boost::filesystem::path repository_root;
  boost::filesystem::path pm_repository_root;
};

struct problem_config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(data);
    ar & BOOST_SERIALIZATION_NVP(root_package);
    ar & BOOST_SERIALIZATION_NVP(strip);
    ar & BOOST_SERIALIZATION_NVP(importer);
  }

  struct {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(archiver);
      ar & BOOST_SERIALIZATION_NVP(filename);
    }

    archiver_options archiver;
    boost::filesystem::path filename;
  } data;

  bunsan::pm::entry root_package;
  bool strip = false;
  boost::property_tree::ptree importer;
};

struct config {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(lock);
    ar & BOOST_SERIALIZATION_NVP(resolver);
    ar & BOOST_SERIALIZATION_NVP(location);
    ar & BOOST_SERIALIZATION_NVP(problem);
    ar & BOOST_SERIALIZATION_NVP(pm);
  }

  boost::filesystem::path lock;
  boost::property_tree::ptree resolver;

  location_config location;

  problem_config problem;

  bunsan::pm::config pm;
};

}  // namespace archive
}  // namespace bacs
