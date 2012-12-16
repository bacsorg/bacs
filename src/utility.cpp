#include "bunsan/utility/utility.hpp"

bunsan::utility::utility::~utility() {}

void bunsan::utility::utility::setup(const boost::property_tree::ptree &/*ptree*/)
{
    BOOST_THROW_EXCEPTION(error("not implemented"));
}
