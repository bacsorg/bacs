#ifndef BUNSAN_PM_ERROR_HPP
#define BUNSAN_PM_ERROR_HPP

#include "bunsan/error.hpp"

namespace bunsan{namespace pm
{
	struct error: virtual bunsan::error
	{
		error()=default;
		explicit error(const std::string &message_);
	};
}}

#endif //BUNSAN_PM_ERROR_HPP

