#ifndef BUNSAN_UTILITY_MAKER_HPP
#define BUNSAN_UTILITY_MAKER_HPP

#include "bunsan/factory_helper.hpp"
#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/utility.hpp"

namespace bunsan{namespace utility
{
	class maker: public utility
	BUNSAN_FACTORY_BEGIN(maker, const resolver &)
	public:
		virtual void exec(
			const boost::filesystem::path &cwd,
			const std::vector<std::string> &targets)=0;
	BUNSAN_FACTORY_END(maker)
}}

#endif //BUNSAN_UTILITY_MAKER_HPP

