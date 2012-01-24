#include <sstream>

#include <cassert>

#include <boost/property_tree/info_parser.hpp>

#include "bunsan/util.hpp"
#include "bunsan/utility/executor.hpp"

int main()
{
	using namespace boost::property_tree;
	using bunsan::utility::executor;
	using bunsan::process::context;
	typedef std::stringstream ss;
	typedef std::vector<std::string> vs;
	ptree pt;
	context ctx;
	ss in_1("n name\n"
		"d\n"
		"{\n"
		"\texecutable exe\n"
		"\tcurrent_path path\n"
		"}\n");
	read_info(in_1, pt);
	try
	{
		ctx = executor(pt).context();
		assert(false);
	}
	catch (std::exception &e)
	{
		SLOG("caught: "<<e.what());
	}
}

