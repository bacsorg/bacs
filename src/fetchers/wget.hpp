#ifndef SRC_FETCHERS_WGET_HPP
#define SRC_FETCHERS_WGET_HPP

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
	class wget: public fetcher
	{
		resolver m_resolver;
	public:
		explicit wget(const resolver &resolver_);
		virtual void fetch(const std::string &uri, const boost::filesystem::path &dst);
		virtual void setarg(const std::string &key, const std::string &value);
	private:
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_FETCHERS_WGET_HPP

