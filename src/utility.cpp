#include "bunsan/utility/utility.hpp"

bunsan::utility::utility::~utility() {}

void bunsan::utility::utility::setup(const boost::property_tree::ptree &ptree)
{
    if (!ptree.empty())
        BOOST_THROW_EXCEPTION(error("not implemented"));
}
