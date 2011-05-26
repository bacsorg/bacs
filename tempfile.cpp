#include "tempfile.hpp"

//#include <boost/filesystem/fstream.hpp>

#include "util.hpp"

its::tempfile::tempfile(): file(boost::filesystem::unique_path(boost::filesystem::temp_directory_path()/"%%%%-%%%%-%%%%-%%%%"))
{
	//boost::filesystem::ofstream reset(file);
}

its::tempfile::tempfile(const boost::filesystem::path &model): file(boost::filesystem::unique_path(model))
{
	//boost::filesystem::ofstream reset(file);
}

/*its::tempfile::operator boost::filesystem::path() const
{
	return file;
}*/

boost::filesystem::path its::tempfile::path() const
{
	return file;
}

std::string its::tempfile::native() const
{
	return file.native();
}

its::tempfile::~tempfile() throw()
{
	try
	{
		SLOG("trying to remove tempfile "<<file);
		boost::filesystem::remove_all(file);
	}
	catch(std::exception &e)
	{
		DLOG(error);
		SLOG(e.what());
		SLOG("unable to remove "<<file);
	}
	catch(...)
	{
		DLOG(unknown error);
		SLOG("unable to remove "<<file);
	}
}

