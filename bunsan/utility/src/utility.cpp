#include <bunsan/utility/utility.hpp>

namespace bunsan::utility {

utility::~utility() {}

void utility::setup(const boost::property_tree::ptree &ptree) {
  if (!ptree.empty()) BOOST_THROW_EXCEPTION(error("not implemented"));
}

}  // namespace bunsan::utility
