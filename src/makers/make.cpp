#include "make.hpp"

#include "bunsan/process/execute.hpp"

#include <boost/lexical_cast.hpp>
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
    for (const auto &i: m_defines)
    {
        // TODO arguments check
        argv.push_back(i.first+"="+i.second);
    }
    if (m_threads)
        argv.push_back("-j"+boost::lexical_cast<std::string>(m_threads.get()));
    argv.insert(argv.end(), m_targets.begin(), m_targets.end());
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

/*!
\verbatim
defines
{
    DESTDIR /some/path
}
targets
{
    "" install
}
threads 3
\endverbatim
*/
void makers::make::setup(const utility::config_type &config)
{
    m_defines.clear();
    m_targets.clear();
    m_threads.reset();
    for (const auto &i: config)
    {
        if (i.first=="defines")
            for (const auto &j: i.second)
                m_defines[j.first] = j.second.get_value<std::string>();
        else if (i.first=="targets")
            for (const auto &j: i.second)
                m_targets.push_back(j.second.get_value<std::string>());
        else if (i.first=="threads")
            m_threads = i.second.get_value<std::size_t>();
        else
            BOOST_THROW_EXCEPTION(unknown_option_error(i.first));
    }
}
