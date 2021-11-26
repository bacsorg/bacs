#include "meson.hpp"
#include "../makers/ninja.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

#include <algorithm>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_builders_meson, {
  BUNSAN_FACTORY_REGISTER_TOKEN(builder, meson, [](const utility_config &ptree, resolver &resolver_) {
    return builder::make_shared<builders::meson>(ptree, resolver_);
  })
})

builders::meson::meson(const utility_config &ptree, resolver &resolver_)
    : m_config(bunsan::config::load<config>(ptree)),
      m_meson_exe(resolver_.find_executable("meson")),
      m_maker(maker::instance("ninja", m_config.maker, resolver_)) {
}

std::vector<std::string> builders::meson::arguments_(
    const boost::filesystem::path &src) const {
  std::vector<std::string> arguments;
  arguments.push_back(m_meson_exe.filename().string());
  for (const auto &i : m_config.meson.flags) {
    if (i.second.empty()) {
      arguments.push_back("--" + i.first);
    } else {
      arguments.push_back("--" + i.first + "=" + i.second);
    }
  }
  for (const auto &i : m_config.meson.options) {
    // TODO arguments check
    arguments.push_back("-D" + i.first + "=" + i.second);
  }
  arguments.push_back(boost::filesystem::absolute(src).string());
  return arguments;
}

void builders::meson::configure_(const boost::filesystem::path &src,
                                 const boost::filesystem::path &bin) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_meson_exe);
    ctx.current_path(bin);
    ctx.arguments(arguments_(src));
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(conf_make_install_configure_error()
                          << conf_make_install_configure_error::src(src)
                          << conf_make_install_configure_error::bin(bin)
                          << enable_nested_current());
  }
}

void builders::meson::make_(const boost::filesystem::path & /*src*/,
                            const boost::filesystem::path &bin) {
  try {
    m_maker->exec(bin, {}, {});
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(conf_make_install_make_error()
                          //<< conf_make_install_make_error::src(src)
                          << conf_make_install_make_error::bin(bin)
                          << enable_nested_current());
  }
}

void builders::meson::install_(const boost::filesystem::path & /*src*/,
                               const boost::filesystem::path &bin,
                               const boost::filesystem::path &root) {
  try {
    m_maker->exec(bin, {"install"}, {{"DESTDIR", root.string()}});
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(conf_make_install_install_error()
                          //<< conf_make_install_install_error::src(src)
                          << conf_make_install_install_error::bin(bin)
                          << conf_make_install_install_error::root(root)
                          << enable_nested_current());
  }
}
