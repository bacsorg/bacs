#ifndef BUNSAN_UTILITY_ERROR_HPP
#define BUNSAN_UTILITY_ERROR_HPP

#include "bunsan/error.hpp"

namespace bunsan{namespace utility
{
	struct error: virtual bunsan::error
	{
		error()=default;
		explicit error(const std::string &msg);
	};
}}

#endif //BUNSAN_UTILITY_ERROR_HPP

