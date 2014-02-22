#include "make.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool makers::make::factory_reg_hook = maker::register_new("make",
    [](const resolver &resolver_)
    {
        maker_ptr ptr(new make(resolver_.find_executable("make")));
        return ptr;
    });

makers::make::make(const boost::filesystem::path &exe): m_exe(exe) {}

std::vector<std::string> makers::make::arguments_(
    const std::vector<std::string> &targets) const
{
    std::vector<std::string> arguments;
    arguments.push_back(m_exe.filename().string());
    for (const auto &i: m_config.defines)
    {
        // TODO arguments check
        arguments.push_back(i.first + "=" + i.second);
    }
    if (m_config.jobs)
        arguments.push_back("-j" + boost::lexical_cast<std::string>(m_config.jobs.get()));
    arguments.insert(arguments.end(), m_config.targets.begin(), m_config.targets.end());
    arguments.insert(arguments.end(), targets.begin(), targets.end());
    return arguments;
}

void makers::make::exec(
    const boost::filesystem::path &cwd,
    const std::vector<std::string> &targets)
{
    try
    {
        bunsan::process::context ctx;
        ctx.executable(m_exe);
        ctx.current_path(cwd);
        ctx.arguments(arguments_(targets));
        check_sync_execute_with_output(ctx);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(maker_exec_error() <<
                              maker_exec_error::cwd(cwd) <<
                              maker_exec_error::targets(targets) <<
                              enable_nested_current());
    }
}

void makers::make::setup(const boost::property_tree::ptree &ptree)
{
    m_config = bunsan::config::load<config>(ptree);
}
