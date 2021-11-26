#include "single.hpp"

#include <bacs/problem/error.hpp>
#include <bacs/problem/split.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs::problem::utilities {

BUNSAN_STATIC_INITIALIZER(bacs_problem_utilities_single, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      utility, single,
      [](const boost::filesystem::path &location,
         const boost::property_tree::ptree &config) {
        return utility::make_shared<single>(location, config);
      })
})

single::single(const boost::filesystem::path &location,
               const boost::property_tree::ptree &config)
    : utility(location, config),
      m_source(config.get<std::string>("build.source")),
      m_std(config.get_optional<std::string>("build.std")),
      m_libs(split::get_vector(config, "build.libs")) {}

namespace {
std::string get_lang(const std::string &source) {
  const std::string::size_type dot = source.rfind('.');
  if (dot == std::string::npos) return "unknown";
  const std::string suff = source.substr(dot + 1);
  if (suff == "c") {
    return "c";
  } else if (suff == "cpp" || suff == "cc") {
    return "cpp";
  } else if (suff == "pas" || suff == "dpr") {
    return "pas";
  } else if (suff == "java") {
    return "java";
  } else if (suff == "py") {
    return "py";
  } else {
    return "unknown";
  }
}
}  // namespace

bool single::make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry & /*package*/,
                          const Revision & /*revision*/) const {
  try {
    boost::filesystem::create_directories(destination);
    bunsan::pm::index index;
    const std::string lang = get_lang(m_source.filename().string());
    // builder itself
    index.source.import.source.push_back(
        {".", "bacs/system/utility/single/" + lang});
    // sources, note: only build section is needed from config
    index.source.self.push_back({"src", "src"});
    bunsan::filesystem::copy_tree(location(), destination / "src");
    // utility configuration
    index.package.self.push_back({"etc", "etc"});
    boost::filesystem::create_directory(destination / "etc");
    boost::property_tree::write_ini((destination / "etc" / target()).string(),
                                    section("call"));
    // modules: set binary name
    index.source.self.push_back({"modules", "modules"});
    boost::filesystem::create_directory(destination / "modules");
    bunsan::filesystem::ofstream fout(destination / "modules" /
                                      "utility.cmake");
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
      fout << "set(target " << target().string() << ")\n";
      fout << "set(source " << m_source.string() << ")\n";
      fout << "set(libraries";
      for (const std::string &lib : m_libs) fout << " " << lib;
      fout << ")\n";
      if (m_std) fout << "set(std " << m_std << ")\n";
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
    // dependencies
    for (const std::string &lib : m_libs) {
      index.source.import.package.push_back(
          {".", "bacs/lib/" + lang + "/" + lib});
    }
    // save it
    index.save(destination / "index");
    return true;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        utility_make_package_error()
        << utility_make_package_error::destination(destination)
        // << utility_make_package_error::package(package)
        << bunsan::enable_nested_current());
  }
}

}  // namespace bacs::problem::utilities
