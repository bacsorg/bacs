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
    use_path 1
    executable exe
    current_path path
})EOF");
    read_info(in_1, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable()=="exe");
    assert(ctx.current_path()=="path");
    assert(ctx.use_path());
    ss in_2(R"EOF(
t 0
d
{
    executable exe
    use_path 0
})EOF");
    read_info(in_2, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable().filename()=="exe" && ctx.executable().is_absolute());
    assert(!ctx.use_path());
    ss in_3(R"EOF(
t 0
d
{
    executable /bin/exe
})EOF");
    read_info(in_3, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable()==boost::filesystem::absolute("/bin/exe"));
    assert(!ctx.use_path());
}
