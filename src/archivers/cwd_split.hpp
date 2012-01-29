#ifndef SRC_ARCHIVERS_CWD_SPLIT_HPP
#define SRC_ARCHIVERS_CWD_SPLIT_HPP

#include "bunsan/utility/archiver.hpp"

namespace bunsan{namespace utility{namespace archivers
{
	class cwd_split: public archiver
	{
	public:
		virtual void pack(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &file);
		virtual void pack_content(
			const boost::filesystem::path &archive,
			const boost::filesystem::path &dir);
	protected:
		virtual void pack_from(
			const boost::filesystem::path &cwd,
			const boost::filesystem::path &archive,
			const boost::filesystem::path &file)=0;
	};
}}}

#endif //SRC_ARCHIVERS_CWD_SPLIT_HPP

