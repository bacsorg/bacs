#include "cmake.hpp"
#include "../makers/make.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/process/execute.hpp"

#include <algorithm>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool builders::cmake::factory_reg_hook = builder::register_new("cmake",
    [](const resolver &resolver_)
    {
        builder_ptr ptr(new cmake(resolver_));
        return ptr;
    });

void builders::cmake::set_default_generator()
{
#if defined(BOOST_POSIX_API)
    set_generator("Unix Makefiles");
#elif defined(BOOST_WINDOWS_API)
    set_generator("NMake Makefiles");
#else
#warning unknown platform, unknown default generator
    BOOST_THROW_EXCEPTION(cmake_unknown_platform_generator_error() <<
                          cmake_unknown_platform_generator_error::message(
                              "Unknown platform, you have to specify generator"));
#endif
}

enum class builders::cmake::generator::type: std::size_t
{
    makefile,
    nmakefile,
    visual_studio,
    unknown
};

const std::vector<builders::cmake::generator> builders::cmake::generators = {
    // makefiles
    {"Unix Makefiles", generator::type::makefile},
    {"MinGW Makefiles", generator::type::makefile},
    {"MSYS Makefiles", generator::type::makefile},
    {"NMake Makefiles", generator::type::makefile},
    {"NMake Makefiles JOM", generator::type::makefile},
    {"Borland Makefiles", generator::type::makefile},
    {"Watcom WMake", generator::type::unknown},
    // IDEs
    // CodeBlocks
    {"CodeBlocks - Unix Makefiles", generator::type::makefile},
    {"CodeBlocks - MinGW Makefiles", generator::type::makefile},
    {"CodeBlocks - NMake Makefiles", generator::type::nmakefile},
    // Eclipse
    {"Eclipse CDT4 - Unix Makefiles", generator::type::makefile},
    {"Eclipse CDT4 - MinGW Makefiles", generator::type::makefile},
    {"Eclipse CDT4 - NMake Makefiles", generator::type::nmakefile},
    // KDevelop
    {"KDevelop3", generator::type::unknown},
    {"KDevelop3 - Unix Makefiles", generator::type::makefile},
    // Visual Studio
    {"Visual Studio 10", generator::type::visual_studio},
    {"Visual Studio 10 IA64", generator::type::visual_studio},
    {"Visual Studio 10 Win64", generator::type::visual_studio},
    {"Visual Studio 11", generator::type::visual_studio},
    {"Visual Studio 11 Win64", generator::type::visual_studio},
    {"Visual Studio 6", generator::type::visual_studio},
    {"Visual Studio 7", generator::type::visual_studio},
    {"Visual Studio 7 .NET 2003", generator::type::visual_studio},
    {"Visual Studio 8 2005", generator::type::visual_studio},
    {"Visual Studio 8 2005 Win64", generator::type::visual_studio},
    {"Visual Studio 9 2008", generator::type::visual_studio},
    {"Visual Studio 9 2008 IA64", generator::type::visual_studio},
    {"Visual Studio 9 2008 Win64", generator::type::visual_studio}
};

builders::cmake::cmake(const resolver &resolver_):
    m_resolver(resolver_), m_cmake_exe(m_resolver.find_executable("cmake"))
{
    set_default_generator();
}

std::vector<std::string> builders::cmake::argv_(const boost::filesystem::path &src) const
{
    std::vector<std::string> argv;
    argv.push_back(m_cmake_exe.filename().string());
    if (m_generator)
    {
        argv.push_back("-G");
        argv.push_back(get_generator().m_id);
    }
    for (const auto &i: m_config.cmake.defines)
    {
        // TODO arguments check
        argv.push_back("-D" + i.first + "=" + i.second);
    }
    argv.push_back(boost::filesystem::absolute(src).string());
    return argv;
}

void builders::cmake::configure_(
    const boost::filesystem::path &src,
    const boost::filesystem::path &bin)
{
    bunsan::process::context ctx;
    ctx.executable(m_cmake_exe);
    ctx.current_path(bin);
    ctx.argv(argv_(src));
    bunsan::process::check_sync_execute(ctx);
}

void builders::cmake::make_(
    const boost::filesystem::path &/*src*/,
    const boost::filesystem::path &bin)
{
    BOOST_ASSERT(m_generator);
    switch (generators[m_generator.get()].m_type)
    {
    case generator::type::makefile:
        {
            maker_ptr ptr = maker::instance("make", m_resolver);
            ptr->setup(m_config.make_maker);
            ptr->exec(bin, {});
        }
        break;
    case generator::type::nmakefile:
    case generator::type::visual_studio:
    default:
        BOOST_THROW_EXCEPTION(cmake_unknown_generator_type_error());
    }
}

void builders::cmake::install_(
    const boost::filesystem::path &/*src*/,
    const boost::filesystem::path &bin,
    const boost::filesystem::path &root)
{
    BOOST_ASSERT(m_generator);
    switch (generators[m_generator.get()].m_type)
    {
    case generator::type::makefile:
        {
            maker_ptr ptr = maker::instance("make", m_resolver);
            makers::make::config make_config = bunsan::config::load<makers::make::config>(m_config.install_maker);
            make_config.defines["DESTDIR"] = boost::filesystem::absolute(root).string();
            ptr->setup(bunsan::config::save<boost::property_tree::ptree>(make_config));
            ptr->exec(bin, {"install"});
        }
        break;
    case generator::type::nmakefile:
    case generator::type::visual_studio:
    default:
        BOOST_THROW_EXCEPTION(cmake_unknown_generator_type_error());
    }
}

void builders::cmake::set_generator(const std::string &gen_id)
{
    const std::size_t gen =
        std::find_if(generators.begin(), generators.end(),
            [&gen_id](const generator &gen)
            {
                return gen.m_id == gen_id;
            }) - generators.begin();
    if (gen >= generators.size())
        BOOST_THROW_EXCEPTION(cmake_unknown_generator_name_error() <<
                              cmake_unknown_generator_name_error::generator_name(gen_id));
    m_generator = gen;
}

const builders::cmake::generator &builders::cmake::get_generator() const
{
    BOOST_ASSERT(m_generator);
    return generators[m_generator.get()];
}

void builders::cmake::setup(const boost::property_tree::ptree &ptree)
{
    m_config = bunsan::config::load<config>(ptree);
    if (m_config.cmake.generator)
        set_generator(m_config.cmake.generator.get());
    else
        set_default_generator();
}
