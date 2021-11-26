#pragma once

#include <bunsan/utility/fetcher.hpp>

namespace bunsan::utility::fetchers {

class curl : public fetcher {
 public:
  explicit curl(const boost::filesystem::path &exe);

  void fetch(const std::string &uri,
             const boost::filesystem::path &dst) override;

 private:
  const boost::filesystem::path m_exe;
};

}  // namespace bunsan::utility::fetchers
