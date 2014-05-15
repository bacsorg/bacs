#include "single.hpp"

#include <bacs/problem/error.hpp>
#include <bacs/problem/split.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace problem{namespace utilities
{
    const bool single::factory_reg_hook = utility::register_new("single",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new single(location, config));
            return tmp;
        });

    single::single(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config):
        utility(location, config),
        m_source(config.get<std::string>("build.source")),
        m_std(config.get_optional<std::string>("build.std")),
        m_libs(split::get_vector(config, "build.libs")) {}

    namespace
    {
        std::string get_lang(const std::string &source)
        {
            const std::string::size_type dot = source.rfind('.');
            if (dot == std::string::npos)
                return "unknown";
            const std::string suff = source.substr(dot + 1);
            if (suff == "c")
                return "c";
            else if (suff == "cpp" || suff == "cc")
                return "cpp";
            else if (suff == "pas" || suff == "dpr")
                return "pas";
            else if (suff == "java")
                return "java";
            else
                return "unknown";
        }
    }

    bool single::make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &/*package*/) const
    {
        try
        {
            boost::filesystem::create_directories(destination);
            bunsan::pm::index index;
            const std::string lang = get_lang(m_source.filename().string());
            // builder itself
            index.source.import.source.insert(
                std::make_pair(".", "bacs/system/utility/single/" + lang)
            );
            // sources, note: only build section is needed from config
            index.source.self.insert(std::make_pair("src", "src"));
            bunsan::filesystem::copy_tree(location(), destination / "src");
            // modules: set binary name
            index.source.self.insert(std::make_pair("modules", "modules"));
            boost::filesystem::create_directory(destination / "modules");
            bunsan::filesystem::ofstream fout(
                destination / "modules" / "utility.cmake");
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout)
            {
                fout << "set(target " << target().string() << ")\n";
                fout << "set(source " << m_source.string() << ")\n";
                fout << "set(libraries";
                for (const std::string &lib: m_libs)
                    fout << " " << lib;
                fout << ")\n";
                if (m_std)
                    fout << "set(std " << m_std << ")\n";
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
            fout.close();
            // dependencies
            for (const std::string &lib: m_libs)
            {
                index.source.import.package.insert(
                    std::make_pair(".", "bacs/lib/" + lang + "/" + lib)
                );
            }
            // save it
            index.save(destination / "index");
            return true;
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(
                utility_make_package_error() <<
                utility_make_package_error::destination(destination) <<
                //utility_make_package_error::package(package) <<
                bunsan::enable_nested_current());
        }
    }
}}}
