#include "7z.hpp"

#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

#ifdef BUNSAN_UTILITY_ARCHIVER_7Z
#   error ASSERTION: BUNSAN_UTILITY_ARCHIVER_7Z is in use
#endif
#define BUNSAN_UTILITY_ARCHIVER_7Z(EXE) \
bool archivers::_7z::factory_reg_hook_##EXE = archiver::register_new(#EXE, \
    [](const resolver &resolver_) \
    { \
        archiver_ptr ptr(new _7z(resolver_.find_executable(#EXE))); \
        return ptr; \
    });

BUNSAN_UTILITY_ARCHIVER_7Z(7z)
BUNSAN_UTILITY_ARCHIVER_7Z(7za)
BUNSAN_UTILITY_ARCHIVER_7Z(7zr)

archivers::_7z::_7z(const boost::filesystem::path &exe):
    m_exe(exe), m_format("7z") {}

void archivers::_7z::pack_from(
    const boost::filesystem::path &cwd,
    const boost::filesystem::path &archive,
    const boost::filesystem::path &file)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.current_path(cwd);
    ctx.argv({
            m_exe.filename().string(),
            "a",
            "-t"+m_format,
            "--",
            archive.string(),
            file.string()
        });
    bunsan::process::check_sync_execute(ctx);
}

void archivers::_7z::unpack(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &dir)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.argv({
            m_exe.filename().string(),
            "x",
            "-t"+m_format,
            "-o"+dir.string(),
            "--",
            archive.string(),
        });
    bunsan::process::check_sync_execute(ctx);
}

void archivers::_7z::setarg(const std::string &key, const std::string &value)
{
    if (key=="format")
        m_format = value;
    else
        BOOST_THROW_EXCEPTION(unknown_option_error(key));
}

