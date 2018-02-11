#include <bacs/problem/error.hpp>
#include <bacs/problem/statement.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace bacs {
namespace problem {
namespace statement_versions {

struct copy_error : virtual error {};

struct invalid_source_name_error : virtual copy_error {
  using source_name =
      boost::error_info<struct tag_source_name, boost::filesystem::path>;
};

class copy : public statement::version {
 public:
  copy(const boost::filesystem::path &location,
       const boost::property_tree::ptree &config);

  void make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package,
                    const bunsan::pm::entry &resources_package,
                    const Revision &revision) const override;

 private:
  const boost::filesystem::path m_source;
};

}  // namespace statement_versions
}  // namespace problem
}  // namespace bacs
