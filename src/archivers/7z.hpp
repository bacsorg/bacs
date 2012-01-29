#ifndef SRC_ARCHIVERS_7Z_HPP
#define SRC_ARCHIVERS_7Z_HPP

#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility{namespace archivers
{
	class _7z: public archiver
	{
		const boost::filesystem::path m_exe;
		std::string m_format;
	public:
		explicit _7z(const boost::filesystem::path &exe);
		virtual void pack(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &dir);
		virtual void unpack(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &dir);
		virtual void setarg(const std::string &key, const std::string &value);
	private:
		static bool factory_reg_hook_7z;
		static bool factory_reg_hook_7za;
		static bool factory_reg_hook_7zr;
	};
}}}

#endif //SRC_ARCHIVERS_7Z_HPP

