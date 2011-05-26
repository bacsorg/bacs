#ifndef TEMPFILE_HPP
#define TEMPFILE_HPP

#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

namespace its
{
	class tempfile
	{
	public:
		tempfile();
		explicit tempfile(const boost::filesystem::path &model);
		//operator boost::filesystem::path() const;
		boost::filesystem::path path() const;
		std::string native() const;
		~tempfile() throw();
	private:
		boost::filesystem::path file;
	};
}

#endif //TEMPFILE_HPP

