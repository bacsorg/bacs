#include <bacs/problem/single/generator.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs::problem::single::generators {

class internal0 : public generator {
 public:
  explicit internal0(const boost::property_tree::ptree &config);

  Problem generate(const options &options_) override;
};

}  // namespace bacs::problem::single::generators
