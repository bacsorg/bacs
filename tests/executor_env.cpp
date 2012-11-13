#include "bunsan/utility/executor.hpp"

#include <sstream>

#include <cassert>

#include <boost/property_tree/info_parser.hpp>

int main()
{
    using namespace boost::property_tree;
    using bunsan::utility::executor;
    using bunsan::process::context;
    typedef std::stringstream ss;
    typedef std::vector<std::string> vs;
    ptree pt;
    context ctx;
    ss in_1(R"EOF(
t 0
d
{
    executable exe
    current_path path
})EOF");
    read_info(in_1, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable()=="exe");
    assert(ctx.current_path()=="path");
}
