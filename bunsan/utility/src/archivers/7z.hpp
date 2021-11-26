#pragma once

#include "cwd_split.hpp"

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan::utility::archivers {

class _7z : public cwd_split {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(format);
    }

    boost::optional<std::string> format;
  };

 public:
  _7z(const utility_config &ptree, const boost::filesystem::path &exe);

  void unpack(const boost::filesystem::path &archive,
              const boost::filesystem::path &dir) override;

 protected:
  void pack_from(const boost::filesystem::path &cwd,
                 const boost::filesystem::path &archive,
                 const boost::filesystem::path &file) override;

 private:
  boost::optional<std::string> format_argument() const;

 private:
  const config m_config;
  const boost::filesystem::path m_exe;
};

}  // namespace bunsan::utility::archivers
