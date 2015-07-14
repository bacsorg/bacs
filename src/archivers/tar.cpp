#include "tar.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_archivers_tar, {
  BUNSAN_FACTORY_REGISTER_TOKEN(archiver, tar, [](resolver &resolver_) {
    return archiver::make_shared<archivers::tar>(
        resolver_.find_executable("tar"));
  })
})

archivers::tar::tar(const boost::filesystem::path &exe) : m_exe(exe) {}

void archivers::tar::pack_from(const boost::filesystem::path &cwd,
                               const boost::filesystem::path &archive,
                               const boost::filesystem::path &file) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.arguments(m_exe.filename(), "--create", format_argument(), "--file",
                  archive, "--directory", cwd, flag_arguments(), "--", file);
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(cwd_split_pack_from_error()
                          << cwd_split_pack_from_error::cwd(cwd)
                          << cwd_split_pack_from_error::archive(archive)
                          << cwd_split_pack_from_error::file(file)
                          << enable_nested_current());
  }
}

void archivers::tar::unpack(const boost::filesystem::path &archive,
                            const boost::filesystem::path &dir) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.arguments(m_exe.filename(), "--extract", format_argument(), "--file",
                  boost::filesystem::absolute(archive), "--directory",
                  boost::filesystem::absolute(dir), flag_arguments());
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(archiver_unpack_error()
                          << archiver_unpack_error::archive(archive)
                          << archiver_unpack_error::file(dir)
                          << enable_nested_current());
  }
}

boost::optional<std::string> archivers::tar::format_argument() const {
  if (m_config.format)
    return (m_config.format->size() == 1 ? "-" : "--") + m_config.format.get();
  return boost::optional<std::string>();
}

std::vector<std::string> archivers::tar::flag_arguments() const {
  std::vector<std::string> argv_;
  for (const config::flag i : m_config.flags) {
    std::string arg = boost::lexical_cast<std::string>(i);
    boost::algorithm::replace_all(arg, "_", "-");
    argv_.push_back("--" + arg);
  }
  return argv_;
}

void archivers::tar::setup(const boost::property_tree::ptree &ptree) {
  m_config = bunsan::config::load<config>(ptree);
}
