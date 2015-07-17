#include <bacs/problem/utility.hpp>

#include <bunsan/static_initializer.hpp>

namespace bacs {
namespace problem {
namespace utilities {

class none : public utility {
 public:
  none(const boost::filesystem::path &location,
       const boost::property_tree::ptree &config)
      : utility(location, config) {}

  bool make_package(const boost::filesystem::path & /*destination*/,
                    const bunsan::pm::entry & /*package*/) const override {
    return false;
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
