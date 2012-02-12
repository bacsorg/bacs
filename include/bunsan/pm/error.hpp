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
	struct invalid_configuration: virtual error
	{
		invalid_configuration()=default;
		explicit invalid_configuration(const std::string &message_);
	};
	struct invalid_configuration_path: virtual invalid_configuration
	{
		invalid_configuration_path=default();
		// tags
		typedef boost::error_info<struct tag_path, std::string> path;
	};
}}

#endif //BUNSAN_PM_ERROR_HPP

