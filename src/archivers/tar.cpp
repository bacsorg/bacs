#include "tar.hpp"

#include "bunsan/utility/detail/join.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/process/execute.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool archivers::tar::factory_reg_hook = archiver::register_new("tar",
    [](const resolver &resolver_)
    {
        archiver_ptr ptr(new tar(resolver_.find_executable("tar")));
        return ptr;
    });

archivers::tar::tar(const boost::filesystem::path &exe): m_exe(exe) {}

void archivers::tar::pack_from(
    const boost::filesystem::path &cwd,
    const boost::filesystem::path &archive,
    const boost::filesystem::path &file)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.argv(detail::join<std::string>(
        m_exe.filename().string(),
        "--create",
        format_argument(),
        "--file",
        archive.string(),
        "--directory",
        cwd.string(),
        flag_arguments(),
        "--",
        file.string()
    ));
    bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::unpack(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &dir)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.argv(detail::join<std::string>(
        m_exe.filename().string(),
        "--extract",
        format_argument(),
        "--file",
        boost::filesystem::absolute(archive).string(),
        "--directory",
        boost::filesystem::absolute(dir).string(),
        flag_arguments()
    ));
    bunsan::process::check_sync_execute(ctx);
}

boost::optional<std::string> archivers::tar::format_argument() const
{
    if (m_config.format)
        return (m_config.format->size() == 1 ? "-" : "--") + m_config.format.get();
    return boost::optional<std::string>();
}

std::vector<std::string> archivers::tar::flag_arguments() const
{
    std::vector<std::string> argv_;
    for (const config::flag i: m_config.flags)
    {
        std::string arg = boost::lexical_cast<std::string>(i);
        boost::algorithm::replace_all(arg, "_", "-");
        argv_.push_back("--" + arg);
    }
    return argv_;
}

void archivers::tar::setup(const boost::property_tree::ptree &ptree)
{
    m_config = bunsan::config::load<config>(ptree);
}
