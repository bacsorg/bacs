#include "bunsan/utility/utility.hpp"

bunsan::utility::utility::~utility() {}

void bunsan::utility::utility::setup(const utility::config_type &config)
{
	for (const auto &i: config)
		setarg(i.first, i.second.get_value<std::string>());
}

