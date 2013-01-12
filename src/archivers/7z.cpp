#include "7z.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

#ifdef BUNSAN_UTILITY_ARCHIVER_7Z
#   error ASSERTION: BUNSAN_UTILITY_ARCHIVER_7Z is in use
#endif
#define BUNSAN_UTILITY_ARCHIVER_7Z(EXE) \
const bool archivers::_7z::factory_reg_hook_##EXE = archiver::register_new(#EXE, \
    [](const resolver &resolver_) \
    { \
        archiver_ptr ptr(new _7z(resolver_.find_executable(#EXE))); \
        return ptr; \
    });

BUNSAN_UTILITY_ARCHIVER_7Z(7z)
BUNSAN_UTILITY_ARCHIVER_7Z(7za)
BUNSAN_UTILITY_ARCHIVER_7Z(7zr)

archivers::_7z::_7z(const boost::filesystem::path &exe): m_exe(exe) {}

void archivers::_7z::pack_from(
    const boost::filesystem::path &cwd,
    const boost::filesystem::path &archive,
    const boost::filesystem::path &file)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.current_path(cwd);
    /*
     * 7z behavior note:
     * When archive.filename() does not contain '.'
     * some suffix is appended depending on format.
     * If archive.filename().back() == '.', last dot is removed.
     * So, in any case we should append '.' to get it work correct.
     */
    ctx.arguments(
        m_exe.filename(),
        "a",
        format_argument(),
        "--",
        archive.string() + ".",
        file
    );
    bunsan::process::check_sync_execute(ctx);
}

void archivers::_7z::unpack(
    const boost::filesystem::path &archive,
    const boost::filesystem::path &dir)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.arguments(
        m_exe.filename(),
        "x",
        format_argument(),
        "-o" + dir.string(),
        "--",
        archive
    );
    bunsan::process::check_sync_execute(ctx);
}

boost::optional<std::string> archivers::_7z::format_argument() const
{
    if (m_config.format)
        return "-t" + m_config.format.get();
    return boost::optional<std::string>();
}

void archivers::_7z::setup(const boost::property_tree::ptree &ptree)
{
    m_config = bunsan::config::load<config>(ptree);
}
