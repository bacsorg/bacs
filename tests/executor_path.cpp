#include <sstream>

#include <cassert>

#include <boost/property_tree/info_parser.hpp>

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
    ss in_1("t 0\n"
        "d\n"
        "{\n"
        "\tuse_path 1\n"
        "\texecutable exe\n"
        "\tcurrent_path path\n"
        "}\n");
    read_info(in_1, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable()=="exe");
    assert(ctx.current_path()=="path");
    assert(ctx.use_path());
    ss in_2("t 0\n"
        "d\n"
        "{\n"
        "\texecutable exe\n"
        "\tuse_path 0\n"
        "}\n");
    read_info(in_2, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable().filename()=="exe" && ctx.executable().is_absolute());
    assert(!ctx.use_path());
    ss in_3("t 0\n"
        "d\n"
        "{\n"
        "\texecutable /bin/exe\n"
        "}\n");
    read_info(in_3, pt);
    ctx = executor(pt).context().built();
    assert(ctx.executable()==boost::filesystem::absolute("/bin/exe"));
    assert(!ctx.use_path());
}

