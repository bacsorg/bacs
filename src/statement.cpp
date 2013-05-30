#include "bacs/problem/statement.hpp"
#include "bacs/problem/error.hpp"

#include "bunsan/filesystem/operations.hpp"

#include "bunsan/pm/index.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace problem
{
    BUNSAN_FACTORY_DEFINE(statement::version)

    statement::version_ptr statement::version::instance(const boost::filesystem::path &config_location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini(config_location.string(), config);
        return instance(config.get<std::string>("build.builder"), config_location.parent_path(), config);
    }

    statement::version::version(const std::string &lang_, const std::string &format_):
        m_lang(lang_), m_format(format_)
    {
        if (!bunsan::pm::entry::is_allowed_subpath(m_lang))
            BOOST_THROW_EXCEPTION(invalid_statement_lang_error() <<
                                  invalid_statement_lang_error::lang(m_lang));
        if (!bunsan::pm::entry::is_allowed_subpath(m_format))
            BOOST_THROW_EXCEPTION(invalid_statement_format_error() <<
                                  invalid_statement_format_error::format(m_format));
    }

    statement::version::~version() {}

    std::string statement::version::lang() const
    {
        return m_lang;
    }

    std::string statement::version::format() const
    {
        return m_format;
    }

    bunsan::pm::entry statement::version::subpackage() const
    {
        return bunsan::pm::entry(lang()) / format();
    }

    namespace
    {
        const bunsan::pm::entry versions_subpackage = "versions";
    }

    statement_ptr statement::instance(const boost::filesystem::path &location)
    {
        const statement_ptr tmp(new statement(location));
        for (boost::filesystem::directory_iterator i(location), end; i != end; ++i)
        {
            if (i->path().filename().extension() == ".ini" &&
                boost::filesystem::is_regular_file(i->path()))
            {
                tmp->m_versions.push_back(version::instance(i->path()));
                pb::problem::Statement::Version &info = *(tmp->m_info.add_versions()) = tmp->m_versions.back()->info();
                const bunsan::pm::entry package = versions_subpackage / info.package();
                info.set_package(package.name());
            }
        }
        return tmp;
    }

    statement::statement(const boost::filesystem::path &location): m_location(location) {}

    bool statement::make_package(const boost::filesystem::path &destination,
                                 const bunsan::pm::entry &package) const
    {
        const bunsan::pm::entry &root_package = package;
        bunsan::filesystem::reset_dir(destination);
        // resources
        const bunsan::pm::entry resources_package = root_package / "resources";
        {
            const boost::filesystem::path package_root = destination / resources_package.location().filename();
            boost::filesystem::create_directory(package_root);
            bunsan::pm::index index;
            index.source.self.insert(std::make_pair(".", "src"));
            bunsan::filesystem::copy_tree(m_location, package_root / "src");
            index.save(package_root / "index");
        }
        // versions
        {
            const boost::filesystem::path versions_path = destination / versions_subpackage.location();
            boost::filesystem::create_directory(versions_path);
            for (const version_ptr &v: m_versions)
            {
                const bunsan::pm::entry package = v->info().package();
                const boost::filesystem::path package_path = versions_path / package.location();
                boost::filesystem::create_directories(package_path);
                v->make_package(package_path,
                                root_package / versions_subpackage / package,
                                resources_package);
            }
        }
        return true;
    }

    const pb::problem::Statement &statement::info() const
    {
        return m_info;
    }
}}
