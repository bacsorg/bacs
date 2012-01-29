#ifndef SRC_FETCHERS_WGET_HPP
#define SRC_FETCHERS_WGET_HPP

#include "bunsan/utility/fetcher.hpp"

namespace bunsan{namespace utility{namespace fetchers
{
	class wget: public fetcher
	{
		const boost::filesystem::path m_exe;
	public:
		explicit wget(const boost::filesystem::path &exe);
		virtual void fetch(const std::string &uri, const boost::filesystem::path &dst);
	private:
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_FETCHERS_WGET_HPP

