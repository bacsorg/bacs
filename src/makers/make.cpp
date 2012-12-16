#include "make.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/process/execute.hpp"

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool makers::make::factory_reg_hook = maker::register_new("make",
    [](const resolver &resolver_)
    {
        maker_ptr ptr(new make(resolver_.find_executable("make")));
        return ptr;
    });

makers::make::make(const boost::filesystem::path &exe): m_exe(exe) {}

std::vector<std::string> makers::make::argv_(
    const std::vector<std::string> &targets) const
{
    std::vector<std::string> argv;
    argv.push_back(m_exe.filename().string());
    for (const auto &i: m_config.defines)
    {
        // TODO arguments check
        argv.push_back(i.first + "=" + i.second);
    }
    if (m_config.jobs)
        argv.push_back("-j" + boost::lexical_cast<std::string>(m_config.jobs.get()));
    argv.insert(argv.end(), m_config.targets.begin(), m_config.targets.end());
    argv.insert(argv.end(), targets.begin(), targets.end());
    return argv;
}

void makers::make::exec(
    const boost::filesystem::path &cwd,
    const std::vector<std::string> &targets)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.current_path(cwd);
    ctx.argv(argv_(targets));
    check_sync_execute(ctx);
}

void makers::make::setup(const boost::property_tree::ptree &ptree)
{
    m_config = bunsan::config::load<config>(ptree);
}
