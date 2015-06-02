#include "cmake.hpp"
#include "../makers/make.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

#include <algorithm>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_builders_cmake,
{
    BUNSAN_FACTORY_REGISTER_TOKEN(builder, cmake,
        [](resolver &resolver_)
        {
            return builder::make_shared<builders::cmake>(resolver_);
        })
})

void builders::cmake::set_default_generator()
{
#if defined(BOOST_POSIX_API)
    set_generator("Unix Makefiles");
#elif defined(BOOST_WINDOWS_API)
    set_generator("NMake Makefiles");
#else
#warning unknown platform, unknown default generator
    BOOST_THROW_EXCEPTION(
        cmake_unknown_platform_generator_error() <<
        cmake_unknown_platform_generator_error::message(
            "Unknown platform, you have to specify generator"));
#endif
}

const std::vector<builders::cmake::generator> builders::cmake::generators = {
    // makefiles
    {"Unix Makefiles", generator_type::MAKEFILE},
    {"MinGW Makefiles", generator_type::MAKEFILE},
    {"MSYS Makefiles", generator_type::MAKEFILE},
    {"NMake Makefiles", generator_type::MAKEFILE},
    {"NMake Makefiles JOM", generator_type::MAKEFILE},
    {"Borland Makefiles", generator_type::MAKEFILE},
    {"Watcom WMake", generator_type::UNKNOWN},
    // IDEs
    // CodeBlocks
    {"CodeBlocks - Unix Makefiles", generator_type::MAKEFILE},
    {"CodeBlocks - MinGW Makefiles", generator_type::MAKEFILE},
    {"CodeBlocks - NMake Makefiles", generator_type::NMAKEFILE},
    // Eclipse
    {"Eclipse CDT4 - Unix Makefiles", generator_type::MAKEFILE},
    {"Eclipse CDT4 - MinGW Makefiles", generator_type::MAKEFILE},
    {"Eclipse CDT4 - NMake Makefiles", generator_type::NMAKEFILE},
    // KDevelop
    {"KDevelop3", generator_type::UNKNOWN},
    {"KDevelop3 - Unix Makefiles", generator_type::MAKEFILE},
    // Visual Studio
    {"Visual Studio 10", generator_type::VISUAL_STUDIO},
    {"Visual Studio 10 IA64", generator_type::VISUAL_STUDIO},
    {"Visual Studio 10 Win64", generator_type::VISUAL_STUDIO},
    {"Visual Studio 11", generator_type::VISUAL_STUDIO},
    {"Visual Studio 11 Win64", generator_type::VISUAL_STUDIO},
    {"Visual Studio 6", generator_type::VISUAL_STUDIO},
    {"Visual Studio 7", generator_type::VISUAL_STUDIO},
    {"Visual Studio 7 .NET 2003", generator_type::VISUAL_STUDIO},
    {"Visual Studio 8 2005", generator_type::VISUAL_STUDIO},
    {"Visual Studio 8 2005 Win64", generator_type::VISUAL_STUDIO},
    {"Visual Studio 9 2008", generator_type::VISUAL_STUDIO},
    {"Visual Studio 9 2008 IA64", generator_type::VISUAL_STUDIO},
    {"Visual Studio 9 2008 Win64", generator_type::VISUAL_STUDIO}
};

builders::cmake::cmake(resolver &resolver_):
    m_resolver(resolver_.clone()),
    m_cmake_exe(resolver_.find_executable("cmake"))
{
    set_default_generator();
}

std::vector<std::string> builders::cmake::arguments_(
    const boost::filesystem::path &src) const
{
    std::vector<std::string> arguments;
    arguments.push_back(m_cmake_exe.filename().string());
    if (m_generator)
    {
        arguments.push_back("-G");
        arguments.push_back(get_generator().name);
    }
    for (const auto &i: m_config.cmake.defines)
    {
        // TODO arguments check
        arguments.push_back("-D" + i.first + "=" + i.second);
    }
    arguments.push_back(boost::filesystem::absolute(src).string());
    return arguments;
}

void builders::cmake::configure_(
    const boost::filesystem::path &src,
    const boost::filesystem::path &bin)
{
    try
    {
        bunsan::process::context ctx;
        ctx.executable(m_cmake_exe);
        ctx.current_path(bin);
        ctx.arguments(arguments_(src));
        bunsan::process::check_sync_execute_with_output(ctx);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            conf_make_install_configure_error() <<
            conf_make_install_configure_error::src(src) <<
            conf_make_install_configure_error::bin(bin) <<
            enable_nested_current());
    }
}

void builders::cmake::make_(
    const boost::filesystem::path &/*src*/,
    const boost::filesystem::path &bin)
{
    BOOST_ASSERT(m_generator);
    try
    {
        const generator_type type = generators[m_generator.get()].type;
        switch (type)
        {
        case generator_type::MAKEFILE:
            {
                const maker_ptr ptr = maker::instance("make", *m_resolver);
                ptr->setup(m_config.make_maker);
                ptr->exec(bin, {});
            }
            break;
        case generator_type::NMAKEFILE:
        case generator_type::VISUAL_STUDIO:
        default:
            BOOST_THROW_EXCEPTION(
                cmake_unknown_generator_type_error() <<
                cmake_unknown_generator_type_error::generator_type(type));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            conf_make_install_make_error() <<
            // conf_make_install_make_error::src(src) <<
            conf_make_install_make_error::bin(bin) <<
            enable_nested_current());
    }
}

void builders::cmake::install_(
    const boost::filesystem::path &/*src*/,
    const boost::filesystem::path &bin,
    const boost::filesystem::path &root)
{
    BOOST_ASSERT(m_generator);
    try
    {
        const generator_type type = generators[m_generator.get()].type;
        switch (type)
        {
        case generator_type::MAKEFILE:
            {
                maker_ptr ptr = maker::instance("make", *m_resolver);
                makers::make::config make_config =
                    bunsan::config::load<makers::make::config>(m_config.install_maker);
                make_config.defines["DESTDIR"] = boost::filesystem::absolute(root).string();
                ptr->setup(bunsan::config::save<boost::property_tree::ptree>(make_config));
                ptr->exec(bin, {"install"});
            }
            break;
        case generator_type::NMAKEFILE:
        case generator_type::VISUAL_STUDIO:
        default:
            BOOST_THROW_EXCEPTION(
                cmake_unknown_generator_type_error() <<
                cmake_unknown_generator_type_error::generator_type(type));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            conf_make_install_install_error() <<
            // conf_make_install_install_error::src(src) <<
            conf_make_install_install_error::bin(bin) <<
            conf_make_install_install_error::root(root) <<
            enable_nested_current());
    }
}

void builders::cmake::set_generator(const std::string &generator_name)
{
    const std::size_t gen =
        std::find_if(generators.begin(), generators.end(),
            [&generator_name](const generator &gen)
            {
                return gen.name == generator_name;
            }) - generators.begin();
    if (gen >= generators.size())
        BOOST_THROW_EXCEPTION(
            cmake_unknown_generator_name_error() <<
            cmake_unknown_generator_name_error::generator_name(generator_name));
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
