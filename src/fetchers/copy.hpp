#ifndef SRC_FETCHERS_COPY_HPP
#define SRC_FETCHERS_COPY_HPP

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
	class copy: public fetcher
	{
	public:
		explicit copy(const resolver &resolver_);
		virtual void fetch(const std::string &uri, const boost::filesystem::path &dst);
		virtual void setarg(const std::string &key, const std::string &value);
	private:
		static bool factory_reg_hook_cp;
		static bool factory_reg_hook_copy;
	};
}}}

#endif //SRC_FETCHERS_COPY_HPP

