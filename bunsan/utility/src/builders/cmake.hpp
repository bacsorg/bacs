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

class cmake : public conf_make_install {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(cmake);
      ar & BOOST_SERIALIZATION_NVP(maker);
    }

    struct {
      template <typename Archive>
      void serialize(Archive &ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(defines);
        ar & BOOST_SERIALIZATION_NVP(generator);
      }

      /// for example {"CMAKE_INSTALL_PREFIX": "/usr"}
      std::unordered_map<std::string, std::string> defines;
      /// for example "Unix Makefiles"
      boost::optional<std::string> generator;
    } cmake;

    boost::property_tree::ptree maker;
  };

 public:
  cmake(const utility_config &ptree, resolver &resolver_);

 protected:
  void configure_(const boost::filesystem::path &src,
                  const boost::filesystem::path &bin) override;

  void make_(const boost::filesystem::path &src,
             const boost::filesystem::path &bin) override;

  void install_(const boost::filesystem::path &src,
                const boost::filesystem::path &bin,
                const boost::filesystem::path &root) override;

 public:
  BUNSAN_INCLASS_STREAM_ENUM_CLASS(generator_type, (MAKEFILE, NMAKEFILE,
                                                    VISUAL_STUDIO))

 private:
  struct generator {
    const std::string name;
    const generator_type type;
  };

 private:
  std::string get_generator() const;
  generator_type get_generator_type() const;
  std::vector<std::string> arguments_(const boost::filesystem::path &src) const;

 private:
  const config m_config;
  const boost::filesystem::path m_cmake_exe;
  maker_ptr m_maker;
};

struct cmake_error : virtual error {};
struct cmake_unknown_generator_error : virtual cmake_error {};

struct cmake_unknown_generator_type_error
    : virtual cmake_unknown_generator_error {
  using generator_type =
      boost::error_info<struct tag_generator_type, cmake::generator_type>;
};

struct cmake_unknown_generator_name_error
    : virtual cmake_unknown_generator_error {
  using generator_name =
      boost::error_info<struct tag_generator_name, std::string>;
};

struct cmake_unknown_platform_generator_error
    : virtual cmake_unknown_generator_error {};

}  // namespace bunsan::utility::builders
