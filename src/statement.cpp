#include <bacs/problem/statement.hpp>

#include <bacs/problem/error.hpp>

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/protobuf/binary.hpp>

#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {

BUNSAN_FACTORY_DEFINE(statement::version)

statement::version_ptr statement::version::instance(
    const boost::filesystem::path &config_location) {
  boost::property_tree::ptree config;
  boost::property_tree::read_ini(config_location.string(), config);
  return instance(config.get<std::string>("build.builder"),
                  config_location.parent_path(), config);
}

statement::version::version(const std::string &language,
                            const std::string &format)
    : m_language(language), m_format(format) {
  if (!bunsan::pm::entry::is_allowed_subpath(m_language))
    BOOST_THROW_EXCEPTION(
        invalid_statement_language_error()
        << invalid_statement_language_error::language(m_language));
  if (!bunsan::pm::entry::is_allowed_subpath(m_format))
    BOOST_THROW_EXCEPTION(invalid_statement_format_error()
                          << invalid_statement_format_error::format(m_format));
  // TODO format -> mime_type?
}

statement::version::~version() {}

std::string statement::version::language() const { return m_language; }

std::string statement::version::format() const { return m_format; }

bunsan::pm::entry statement::version::subpackage() const {
  return bunsan::pm::entry(language()) / format();
}

Statement::Version statement::version::info() const {
  Statement::Version v;
  v.set_language(language());
  v.set_format(format());
  v.set_package(subpackage().name());
  return v;
}

/// \warning keep in sync with bacs/system/statement
const boost::filesystem::path statement::version::manifest_path = "manifest";
const boost::filesystem::path statement::version::data_path = "data";

statement::version::built::built(const boost::filesystem::path &package_root)
    : m_package_root(package_root),
      m_manifest(
          bunsan::protobuf::binary::parse_make<Statement::Version::Manifest>(
              package_root / manifest_path)) {}

boost::filesystem::path statement::version::built::data_root() const {
  return m_package_root / version::data_path;
}

boost::filesystem::path statement::version::built::index() const {
  return data_root() / manifest().data().index();
}

namespace {
const bunsan::pm::entry versions_subpackage = "versions";
}  // namespace

statement_ptr statement::instance(const boost::filesystem::path &location) {
  std::vector<version_ptr> versions;
  for (boost::filesystem::directory_iterator i(location), end; i != end; ++i) {
    if (i->path().filename().extension() == ".ini" &&
        boost::filesystem::is_regular_file(i->path())) {
      versions.push_back(version::instance(i->path()));
    }
  }
  statement_ptr tmp(new statement(location, versions));
  return tmp;
}

statement::statement(const boost::filesystem::path &location,
                     const std::vector<version_ptr> &versions)
    : m_location(location), m_versions(versions) {
  update_info();
}

void statement::update_info() {
  m_info.clear_version();
  for (const version_ptr &version : m_versions) {
    Statement::Version &info = *m_info.add_version() = version->info();
    const bunsan::pm::entry package = versions_subpackage / info.package();
    info.set_package(package.name());
  }
}

bool statement::make_package(const boost::filesystem::path &destination,
                             const bunsan::pm::entry &package,
                             const Revision &revision) const {
  try {
    const bunsan::pm::entry &root_package = package;
    bunsan::filesystem::reset_dir(destination);

    // resources
    const bunsan::pm::entry resources_package = root_package / "resources";
    {
      const boost::filesystem::path package_root =
          destination / resources_package.location().filename();
      boost::filesystem::create_directory(package_root);
      bunsan::pm::index index;
      index.source.self.push_back({".", "src"});
      bunsan::filesystem::copy_tree(m_location, package_root / "src");
      index.save(package_root / "index");
    }

    // versions
    {
      const boost::filesystem::path versions_path =
          destination / versions_subpackage.location();
      boost::filesystem::create_directory(versions_path);
      for (const version_ptr &v : m_versions) {
        const bunsan::pm::entry package = v->info().package();
        const boost::filesystem::path package_path =
            versions_path / package.location();
        boost::filesystem::create_directories(package_path);
        v->make_package(package_path,
                        root_package / versions_subpackage / package,
                        resources_package, revision);
      }
    }

    return true;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        buildable_make_package_error()
        << buildable_make_package_error::destination(destination)
        << buildable_make_package_error::package(package)
        << bunsan::enable_nested_current());
  }
}

const Statement &statement::info() const { return m_info; }

}  // namespace problem
}  // namespace bacs
