#include "cmake.hpp"
#include "../makers/make.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_builders_cmake, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      builder, cmake, [](const utility_config &ptree, resolver &resolver_) {
        return builder::make_shared<builders::cmake>(ptree, resolver_);
      })
})

builders::cmake::cmake(const utility_config &ptree, resolver &resolver_)
    : m_config(bunsan::config::load<config>(ptree)),
      m_cmake_exe(resolver_.find_executable("cmake")) {
  const generator_type type = get_generator_type();
  switch (type) {
    case generator_type::MAKEFILE:
      m_maker = maker::instance("make", m_config.maker, resolver_);
      break;
    default:
      BOOST_THROW_EXCEPTION(
          cmake_unknown_generator_type_error()
          << cmake_unknown_generator_type_error::generator_type(type));
  }
}

std::vector<std::string> builders::cmake::arguments_(
    const boost::filesystem::path &src) const {
  std::vector<std::string> arguments;
  arguments.push_back(m_cmake_exe.filename().string());
  arguments.push_back("-G");
  arguments.push_back(get_generator());
  for (const auto &i : m_config.cmake.defines) {
    // TODO arguments check
    arguments.push_back("-D" + i.first + "=" + i.second);
  }
  arguments.push_back(boost::filesystem::absolute(src).string());
  return arguments;
}

void builders::cmake::configure_(const boost::filesystem::path &src,
                                 const boost::filesystem::path &bin) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_cmake_exe);
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

void builders::cmake::make_(const boost::filesystem::path & /*src*/,
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

void builders::cmake::install_(const boost::filesystem::path & /*src*/,
                               const boost::filesystem::path &bin,
                               const boost::filesystem::path &root) {
  try {
    m_maker->exec(bin, {"install"},
                  {{"DESTDIR", boost::filesystem::absolute(root).string()}});
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(conf_make_install_install_error()
                          //<< conf_make_install_install_error::src(src)
                          << conf_make_install_install_error::bin(bin)
                          << conf_make_install_install_error::root(root)
                          << enable_nested_current());
  }
}

std::string builders::cmake::get_generator() const {
  if (m_config.cmake.generator) {
    return *m_config.cmake.generator;
  } else {
#if defined(BOOST_POSIX_API)
    return "Unix Makefiles";
#elif defined(BOOST_WINDOWS_API)
    return "NMake Makefiles";
#else
#warning unknown platform, unknown default generator
    BOOST_THROW_EXCEPTION(
        cmake_unknown_platform_generator_error()
        << cmake_unknown_platform_generator_error::message(
               "Unknown platform, you have to specify generator"));
#endif
  }
}

builders::cmake::generator_type builders::cmake::get_generator_type() const {
  const std::string generator = get_generator();
  if (generator.find("Makefiles") != std::string::npos) {
    if (generator.find("NMake Makefiles") != std::string::npos) {
      return generator_type::NMAKEFILE;
    }
    return generator_type::MAKEFILE;
  } else if (generator.find("Visual Studio") != std::string::npos) {
    return generator_type::VISUAL_STUDIO;
  } else {
    BOOST_THROW_EXCEPTION(
        cmake_unknown_generator_name_error()
        << cmake_unknown_generator_name_error::generator_name(generator));
  }
}
