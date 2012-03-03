#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/error.hpp"

#include <sstream>

#include <boost/assert.hpp>
#include <boost/property_tree/info_parser.hpp>

static void init(bunsan::utility::resolver &res, const char *cfg)
{
	std::istringstream scfg(cfg);
	boost::property_tree::ptree cfg_;
	boost::property_tree::read_info(scfg, cfg_);
	res = bunsan::utility::resolver(cfg_);
}

static const char cfg_absolute[] = R"(
absolute
{
	foobar /usr/some/foobar
}
)";

static const char cfg_alias[] = R"(
alias
{
	foo foo2
}
absolute
{
	foo2 /usr/some/foo2
}
)";

static const char cfg_long_alias[] = R"(
alias
{
	foo foo2
	bar bar2
	bar2 bar3
	foo2 foo100500
	foo100500 foo3
}
absolute
{
	foo3 /usr/some/foo3
}
)";

static const char cfg_ambigous[] = R"(
alias
{
	foo foo2
}
absolute
{
	foo /usr/some/foo
	foo2 /usr/some/foo2
}
)";

static const char cfg_circular_alias[] = R"(
alias
{
	foo foo2
	foo3 foo
	foo2 foo3
}
)";

int main()
{
	bunsan::utility::resolver res;
	init(res, cfg_absolute);
	BOOST_ASSERT("/usr/some/foobar"==res.find_executable("foobar"));
	BOOST_ASSERT("/some/abs"==res.find_executable("/some/abs"));
	init(res, cfg_alias);
	BOOST_ASSERT("/usr/some/foo2"==res.find_executable("foo"));
	init(res, cfg_long_alias);
	BOOST_ASSERT("/usr/some/foo3"==res.find_executable("foo"));
	init(res, cfg_ambigous);
	BOOST_ASSERT("/usr/some/foo2"==res.find_executable("foo"));
	init(res, cfg_circular_alias);
	try
	{
		res.find_executable("foo");
		BOOST_ASSERT_MSG(false, "exception should be raised");
	}
	catch (bunsan::utility::error &e)
	{
	}
	catch (...)
	{
		BOOST_ASSERT_MSG(false, "unknown exception type");
	}
	// TODO find_library
}

