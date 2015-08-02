#include "copy.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {
namespace statement_versions {

BUNSAN_STATIC_INITIALIZER(bacs_problem_statement_versions_copy, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      statement::version, copy,
      [](const boost::filesystem::path &location,
         const boost::property_tree::ptree &config) {
        return statement::version::make_shared<copy>(location, config);
      })
})

namespace {
std::string get_format(const boost::filesystem::path &source) {
  const std::string filename = source.string();
  const std::size_t pos = filename.rfind('.');
  if (pos == std::string::npos)
    BOOST_THROW_EXCEPTION(invalid_source_name_error()
                          << invalid_source_name_error::source_name(source));
  return filename.substr(pos + 1);
}
}  // namespace

copy::copy(const boost::filesystem::path & /*location*/,
           const boost::property_tree::ptree &config)
    : version(config.get<std::string>("info.language"),
              get_format(config.get<std::string>("build.source"))),
      m_source(config.get<std::string>("build.source")) {}

void copy::make_package(const boost::filesystem::path &destination,
                        const bunsan::pm::entry & /*package*/,
                        const bunsan::pm::entry &resources_package) const {
  try {
    bunsan::filesystem::reset_dir(destination);
    bunsan::pm::index index;
    index.package.import.source.insert(
        std::make_pair("data", resources_package));
    index.package.self.insert(std::make_pair(".", "pkg"));
    boost::filesystem::create_directory(destination / "pkg");
    manifest statement_manifest;
    statement_manifest.version.language = language();
    statement_manifest.version.format = format();
    statement_manifest.data.index = m_source;
    boost::property_tree::write_ini(
        (destination / "pkg" / manifest_path).string(),
        bunsan::config::save<boost::property_tree::ptree>(statement_manifest));
    index.save(destination / "index");
  } catch (std::exception &) {
    typedef statement_version_make_package_error svmp_error;
    BOOST_THROW_EXCEPTION(svmp_error()
                          << svmp_error::destination(destination)
                          // << svmp_error::package(package)
                          << svmp_error::resources_package(resources_package)
                          << bunsan::enable_nested_current());
  }
}

}  // namespace statement_versions
}  // namespace problem
}  // namespace bacs
