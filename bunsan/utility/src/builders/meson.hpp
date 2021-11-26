#pragma once

#include "conf_make_install.hpp"

#include <bunsan/stream_enum.hpp>
#include <bunsan/utility/maker.hpp>

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace bunsan::utility::builders {

class meson : public conf_make_install {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(meson);
      ar & BOOST_SERIALIZATION_NVP(maker);
    }

    struct {
      template <typename Archive>
      void serialize(Archive &ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(options);
        ar & BOOST_SERIALIZATION_NVP(flags);
      }

      /// for example {"prefix": "/usr"}
      /// translated into -Dprefix=/usr
      std::unordered_map<std::string, std::string> options;
      /// for example {"buildtype": "release"}
      /// translated into --buildtype=release
      std::unordered_map<std::string, std::string> flags;
    } meson;

    boost::property_tree::ptree maker;
  };

 public:
  meson(const utility_config &ptree, resolver &resolver_);

 protected:
  void configure_(const boost::filesystem::path &src,
                  const boost::filesystem::path &bin) override;

  void make_(const boost::filesystem::path &src,
             const boost::filesystem::path &bin) override;

  void install_(const boost::filesystem::path &src,
                const boost::filesystem::path &bin,
                const boost::filesystem::path &root) override;

 private:
  std::vector<std::string> arguments_(const boost::filesystem::path &src) const;

 private:
  const config m_config;
  const boost::filesystem::path m_meson_exe;
  const maker_ptr m_maker;
};

struct meson_error : virtual error {};

}  // namespace bunsan::utility::builders
