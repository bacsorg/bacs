#include <bacs/problem/utility.hpp>

#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {
namespace utilities {

class none : public utility {
 public:
  none(const boost::filesystem::path &location,
       const boost::property_tree::ptree &config)
      : utility(location, config) {}

  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry & /*package*/,
                    const Revision & /*revision*/) const override {
    try {
      boost::filesystem::create_directories(destination);
      bunsan::pm::index index;
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
};

BUNSAN_STATIC_INITIALIZER(bacs_problem_utilities_none, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      utility, none,
      [](const boost::filesystem::path &location,
         const boost::property_tree::ptree &config) {
        return utility::make_shared<none>(location, config);
      })
})

}  // namespace utilities
}  // namespace problem
}  // namespace bacs
