#ifndef SRC_ARCHIVERS_TAR_HPP
#define SRC_ARCHIVERS_TAR_HPP

#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility{namespace archivers
{
	class tar: public archiver
	{
		resolver m_resolver;
		std::string m_format;
	public:
		explicit tar(const resolver &resolver_);
		virtual void pack(
			const boost::filesystem::path &src,
			const boost::filesystem::path &dst);
		virtual void unpack(
			const boost::filesystem::path &src,
			const boost::filesystem::path &dst);
		virtual void setarg(const std::string &key, const std::string &value);
	private:
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_ARCHIVERS_TAR_HPP

