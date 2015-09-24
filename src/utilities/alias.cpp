#include <bacs/problem/utility.hpp>

#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {
namespace utilities {

class alias : public utility {
 public:
  alias(const boost::filesystem::path &location,
        const boost::property_tree::ptree &config)
      : utility(location, config),
        m_alias(config.get<std::string>("build.alias")) {}

  boost::property_tree::ptree section(const std::string &name) const override;

  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry & /*package*/,
                    const Revision & /*revision*/) const override;

 private:
  const std::string m_alias;
};

BUNSAN_STATIC_INITIALIZER(bacs_problem_utilities_alias, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      utility, alias,
      [](const boost::filesystem::path &location,
         const boost::property_tree::ptree &config) {
        return utility::make_shared<alias>(location, config);
      })
})

boost::property_tree::ptree alias::section(const std::string &name) const {
  if (name == "utility") {
    boost::property_tree::ptree u;
    u.put("call", "reserved");
    u.put("return", "reserved");
    return u;
  }
  return utility::section(name);
}

bool alias::make_package(const boost::filesystem::path &destination,
                         const bunsan::pm::entry & /*package*/,
                         const Revision & /*revision*/) const {
  try {
    boost::filesystem::create_directories(destination);
    bunsan::pm::index index;
    // alias
    index.package.import.package.insert(
        std::make_pair(".", bunsan::pm::entry("bacs/system/single") /
                                target().string() / "std" / m_alias));
    // utility configuration
    index.package.self.insert(std::make_pair("etc", "etc"));
    boost::filesystem::create_directory(destination / "etc");
    boost::property_tree::write_ini((destination / "etc" / target()).string(),
                                    section("call"));
    // save it
    index.save(destination / "index");
    return true;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        utility_make_package_error()
        << utility_make_package_error::destination(destination)
        // << utility_make_package_error::package(package)
        << bunsan::enable_nested_current());
  }
}
}  // namespace utilities
}  // namespace problem
}  // namespace bacs
