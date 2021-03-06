#include <bacs/problem/utility.hpp>

#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace bacs::problem::utilities {

class dot_net_solution : public utility {
 public:
  dot_net_solution(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config);

  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package,
                    const Revision &revision) const override;

 private:
  const boost::filesystem::path m_solution;
  const boost::optional<std::string> m_configuration;
  const std::vector<std::string> m_libs;
};

}  // namespace bacs::problem::utilities
