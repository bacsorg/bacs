#include "ninja.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_makers_ninja, {
  BUNSAN_FACTORY_REGISTER_TOKEN(maker, ninja, [](resolver &resolver_) {
    return maker::make_shared<makers::ninja>(
        resolver_.find_executable("ninja"));
  })
})

makers::ninja::ninja(const boost::filesystem::path &exe) : m_exe(exe) {}

std::vector<std::string> makers::ninja::arguments_(
    const std::vector<std::string> &targets) const {
  std::vector<std::string> arguments;
  arguments.push_back(m_exe.filename().string());
  if (m_config.jobs)
    arguments.push_back("-j" +
                        boost::lexical_cast<std::string>(m_config.jobs.get()));
  arguments.insert(arguments.end(), m_config.targets.begin(),
                   m_config.targets.end());
  arguments.insert(arguments.end(), targets.begin(), targets.end());
  return arguments;
}

void makers::ninja::exec(
    const boost::filesystem::path &cwd, const std::vector<std::string> &targets,
    const std::unordered_map<std::string, std::string> &flags) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.current_path(cwd);
    ctx.arguments(arguments_(targets));
    for (const auto &i : m_config.defines) {
      // TODO arguments check
      ctx.environment_set(i.first, i.second);
    }
    for (const auto &i : flags) {
      // TODO arguments check
      ctx.environment_set(i.first, i.second);
    }
    check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(maker_exec_error()
                          << maker_exec_error::cwd(cwd)
                          << maker_exec_error::targets(targets)
                          << maker_exec_error::flags(flags)
                          << enable_nested_current());
  }
}

void makers::ninja::setup(const boost::property_tree::ptree &ptree) {
  m_config = bunsan::config::load<config>(ptree);
}
