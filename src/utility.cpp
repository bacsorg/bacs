#include <bunsan/utility/utility.hpp>

namespace bunsan {
namespace utility {

utility::~utility() {}

void utility::setup(const boost::property_tree::ptree &ptree) {
  if (!ptree.empty()) BOOST_THROW_EXCEPTION(error("not implemented"));
}

}  // namespace utility
}  // namespace bunsan
