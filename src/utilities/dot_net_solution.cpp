#include "dot_net_solution.hpp"

#include <bacs/problem/error.hpp>
#include <bacs/problem/split.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace problem{namespace utilities
{
    const bool dot_net_solution::factory_reg_hook =
        utility::register_new("dot_net_solution",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new dot_net_solution(location, config));
            return tmp;
        });

    dot_net_solution::dot_net_solution(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config):
            utility(location, config),
            m_solution(config.get<std::string>("build.solution")),
            m_configuration(
                config.get_optional<std::string>("build.configuration")),
            m_libs(split::get_vector(config, "build.libs")) {}

    bool dot_net_solution::make_package(
        const boost::filesystem::path &destination,
        const bunsan::pm::entry &/*package*/) const
    {
        try
        {
            boost::filesystem::create_directories(destination);
            bunsan::pm::index index;
            // builder itself
            index.source.import.source.insert(
                std::make_pair(".", "bacs/system/utility/dot_net_solution")
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
                fout << "set(solution " << m_solution.string() << ")\n";
                if (m_configuration)
                    fout << "set(configuration " << m_configuration << ")\n";
                fout << "set(libraries";
                for (const std::string &lib: m_libs)
                    fout << " " << lib;
                fout << ")\n";
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
            fout.close();
            // dependencies
            for (const std::string &lib: m_libs)
            {
                index.source.import.package.insert(
                    std::make_pair(".", "bacs/lib/dot_net_solution/" + lib)
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
