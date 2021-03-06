#pragma once

#include <bunsan/utility/maker.hpp>

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace bunsan::utility::makers {

class make : public maker {
 public:
  struct config {
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(defines);
      ar & BOOST_SERIALIZATION_NVP(targets);
      ar & BOOST_SERIALIZATION_NVP(jobs);
    }

    /// for example {"DESTDIR": "/some/path"}
    std::unordered_map<std::string, std::string> defines;
    std::vector<std::string> targets;  ///< for example ["install"]
    boost::optional<std::size_t> jobs;
  };

 public:
  make(const utility_config &ptree, const boost::filesystem::path &exe);

  void exec(const boost::filesystem::path &cwd,
            const std::vector<std::string> &targets,
            const std::unordered_map<std::string, std::string> &flags) override;

 private:
  std::vector<std::string> arguments_(
      const std::vector<std::string> &targets,
      const std::unordered_map<std::string, std::string> &flags) const;

 private:
  const config m_config;
  const boost::filesystem::path m_exe;
};

}  // namespace bunsan::utility::makers
