#include "../execute.hpp"
#include "util.hpp"

#include <sstream>

#include <cstdlib>

#include <sys/wait.h>

template <typename S>
void process(S &out, const std::string &str)
{
	out<<'"';
	for (size_t j = 0; j<str.size(); ++j)
	{
		if (str[j]=='"')
			out<<'\\';
		out<<str[j];
	}
	out<<'"';
}

int its::impl::execute(const boost::filesystem::path &cwd, const std::vector<std::string> &args)
{
	std::stringstream sout;
	DLOG(trying to execute the following args);
	sout<<"cd ";
	process(sout, cwd.native());
	sout<<" && ";
	for (size_t i = 0; i<args.size(); ++i)
	{
		if (i)
			sout<<' ';
		process(sout, args[i]);
	}
	SLOG(sout.str());
	int code = system(sout.str().c_str());
	SLOG("program was finished with \""<<code<<"\" status code: return code=\""<<WEXITSTATUS(code)<<"\"");
	return WEXITSTATUS(code);
}

