#include "7z.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

#define BUNSAN_UTILITY_ARCHIVER_7Z(EXE)                                   \
  BUNSAN_FACTORY_REGISTER_TOKEN(archiver, EXE, [](resolver & resolver_) { \
    return archiver::make_shared<archivers::_7z>(                         \
        resolver_.find_executable(#EXE));                                 \
  })

BUNSAN_STATIC_INITIALIZER(bunsan_utility_archivers_7z, {
  BUNSAN_UTILITY_ARCHIVER_7Z(7z)
  BUNSAN_UTILITY_ARCHIVER_7Z(7za)
  BUNSAN_UTILITY_ARCHIVER_7Z(7zr)
})

archivers::_7z::_7z(const boost::filesystem::path &exe) : m_exe(exe) {}

void archivers::_7z::pack_from(const boost::filesystem::path &cwd,
                               const boost::filesystem::path &archive,
                               const boost::filesystem::path &file) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.current_path(cwd);
    /*
     * 7z behavior note:
     * When archive.filename() does not contain '.'
     * some suffix is appended depending on format.
     * If archive.filename().back() == '.', last dot is removed.
     * So, in any case we should append '.' to make it work properly.
     */
    ctx.arguments(m_exe.filename(), "a", format_argument(), "--",
                  archive.string() + ".", file);
    boost::filesystem::remove(archive);  // 7z does not support overwrite
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cwd_split_pack_from_error()
                          << cwd_split_pack_from_error::cwd(cwd)
                          << cwd_split_pack_from_error::archive(archive)
                          << cwd_split_pack_from_error::file(file)
                          << enable_nested_current());
  }
}

void archivers::_7z::unpack(const boost::filesystem::path &archive,
                            const boost::filesystem::path &dir) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.arguments(m_exe.filename(), "x", format_argument(), "-o" + dir.string(),
                  "--", archive);
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(archiver_unpack_error()
                          << archiver_unpack_error::archive(archive)
                          << archiver_unpack_error::file(dir)
                          << enable_nested_current());
  }
}

boost::optional<std::string> archivers::_7z::format_argument() const {
  if (m_config.format) return "-t" + m_config.format.get();
  return boost::optional<std::string>();
}

void archivers::_7z::setup(const boost::property_tree::ptree &ptree) {
  m_config = bunsan::config::load<config>(ptree);
}
