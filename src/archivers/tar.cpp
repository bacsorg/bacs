#include "tar.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/process/execute.hpp"

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
    std::vector<std::string> argv_ =
        {
            m_exe.filename().string(),
            "c"+m_format+"f",
            archive.string(),
            "-C",
            cwd.string()
        };
    for (const auto &i: m_args)
        argv_.push_back("--"+i);
    argv_.push_back("--");
    argv_.push_back(file.string());
    ctx.argv(argv_);
    bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::unpack(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &dir)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    std::vector<std::string> argv_ =
        {
            m_exe.filename().string(),
            "x"+m_format+"f",
            boost::filesystem::absolute(archive).string(),
            "-C",
            boost::filesystem::absolute(dir).string()
        };
    for (const auto &i: m_args)
        argv_.push_back("--"+i);
    ctx.argv(argv_);
    bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::setarg(const std::string &key, const std::string &value)
{
    if (key=="format")
        m_format = value;
    else if (key=="exclude-vcs")
    {
        if (boost::lexical_cast<unsigned>(value))
            m_args.insert(key);
        else
            m_args.erase(key);
    }
    else
        BOOST_THROW_EXCEPTION(unknown_option_error(key));
}

