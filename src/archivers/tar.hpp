#ifndef SRC_ARCHIVERS_TAR_HPP
#define SRC_ARCHIVERS_TAR_HPP

#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility{namespace archivers
{
	class tar: public archiver
	{
		const boost::filesystem::path m_exe;
		std::string m_format;
	public:
		explicit tar(const boost::filesystem::path &exe);
		virtual void pack(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &dir);
		virtual void unpack(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &dir);
		virtual void setarg(const std::string &key, const std::string &value);
	private:
		static bool factory_reg_hook;
	};
}}}

#endif //SRC_ARCHIVERS_TAR_HPP

