#include "pdflatex.hpp"

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/protobuf/binary.hpp>
#include <bunsan/static_initializer.hpp>
#include <bunsan/tempfile.hpp>

namespace bacs::problem::statement_versions {

BUNSAN_STATIC_INITIALIZER(bacs_problem_statement_versions_pdflatex, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      statement::version, pdflatex,
      [](const boost::filesystem::path &location,
         const boost::property_tree::ptree &config) {
        return statement::version::make_shared<pdflatex>(location, config);
      })
})

namespace {
boost::filesystem::path get_root(const boost::filesystem::path &source) {
  return source.parent_path();
}
boost::filesystem::path get_source(const boost::filesystem::path &source) {
  return source.filename();
}
boost::filesystem::path get_target(const boost::filesystem::path &source) {
  return source.stem().string() + ".pdf";
}
}  // namespace

pdflatex::pdflatex(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config)
    : version(config.get<std::string>("info.language"), "pdf"),
      m_location(location),
      m_root(get_root(config.get<std::string>("build.source"))),
      m_source(get_source(config.get<std::string>("build.source"))),
      m_target(get_target(config.get<std::string>("build.source"))) {}

void pdflatex::make_package(const boost::filesystem::path &destination,
                            const bunsan::pm::entry &package,
                            const bunsan::pm::entry &resources_package,
                            const Revision &revision) const {
  try {
    bunsan::filesystem::reset_dir(destination);
    bunsan::pm::index index;
    index.package.self.push_back({".", "pkg"});
    boost::filesystem::create_directory(destination / "pkg");
    Statement::Version::Manifest statement_manifest;
    statement_manifest.mutable_version()->set_language(language());
    statement_manifest.mutable_version()->set_format(format());
    statement_manifest.mutable_version()->set_package(package.name());
    *statement_manifest.mutable_revision() = revision;
    statement_manifest.mutable_data()->set_index(m_target.string());
    bunsan::protobuf::binary::serialize(statement_manifest,
                                        destination / "pkg" / manifest_path);
    index.save(destination / "index");
    build(destination / "pkg" / m_target);
  } catch (std::exception &) {
    using svmp_error = statement_version_make_package_error;
    BOOST_THROW_EXCEPTION(svmp_error()
                          << svmp_error::destination(destination)
                          // << svmp_error::package(package)
                          << svmp_error::resources_package(resources_package)
                          << bunsan::enable_nested_current());
  }
}

void pdflatex::build(const boost::filesystem::path &destination) const {
  try {
    const bunsan::tempfile tmp = bunsan::tempfile::directory_in_tempdir();
    bunsan::filesystem::copy_tree(m_location, tmp.path());
    bunsan::process::context ctx;
    ctx.executable("pdflatex");
    ctx.arguments("pdflatex", tmp.path() / m_source);
    bunsan::process::check_sync_execute(ctx);  // builds index
    bunsan::process::check_sync_execute(ctx);  // reuses index
    boost::filesystem::create_directories(destination.parent_path());
    boost::filesystem::copy_file(tmp.path() / m_target, destination);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(pdflatex_build_error()
                          << pdflatex_build_error::destination(destination)
                          << bunsan::enable_nested_current());
  }
}

}  // namespace bacs::problem::statement_versions
