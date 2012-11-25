#include "repository_native.hpp"

#include "bunsan/pm/index.hpp"
#include "bunsan/pm/config.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/logging/legacy.hpp"
#include "bunsan/filesystem/operations.hpp"

#include <boost/property_tree/info_parser.hpp>

std::string bunsan::pm::repository::native::value(const std::string &key)
{
    return config.get<std::string>(key);
}

void bunsan::pm::repository::native::read_checksum(const entry &package, boost::property_tree::ptree &ptree)
{
    boost::property_tree::read_info(source_resource(package, value(config::name::file::checksum)).string(), ptree);
}

std::string bunsan::pm::repository::native::remote_resource(const entry &package, const std::string &name)
{
    return package.remote_resource(value(config::repository_url), name);
}

boost::filesystem::path bunsan::pm::repository::native::source_resource(const entry &package, const std::string &name)
{
    return package.local_resource(value(config::dir::source), name);
}

boost::filesystem::path bunsan::pm::repository::native::package_resource(const entry &package, const std::string &name)
{
    return package.local_resource(value(config::dir::package), name);
}

std::multimap<boost::filesystem::path, std::string> bunsan::pm::repository::native::sources(const entry &package)
{
    return read_depends(package).source.self;
}

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_):
    config(config_),
    m_resolver(config.get_child(config::command::resolver))
{
    using boost::property_tree::ptree;
    using namespace config::command;
    std::string type;
    // creation
    if (!(cache_archiver = utility::archiver::instance(type = value(cache_archiver::type), m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_cache_archiver_error() <<
                              invalid_configuration_cache_archiver_error::utility_type(type));

    if (!(source_archiver = utility::archiver::instance(type = value(source_archiver::type), m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_source_archiver_error() <<
                              invalid_configuration_source_archiver_error::utility_type(type));

    if (!(builder = utility::builder::instance(type = value(builder::type), m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_builder_error() <<
                              invalid_configuration_builder_error::utility_type(type));

    if (!(fetcher = utility::fetcher::instance(type = value(fetcher::type), m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_fetcher_error() <<
                              invalid_configuration_fetcher_error::utility_type(type));
    // setup
    cache_archiver->setup(config.get_child(cache_archiver::config, ptree()));
    source_archiver->setup(config.get_child(source_archiver::config, ptree()));
    builder->setup(config.get_child(builder::config, ptree()));
    fetcher->setup(config.get_child(fetcher::config, ptree()));
}

void bunsan::pm::repository::native::write_snapshot(const boost::filesystem::path &path, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
    boost::property_tree::ptree snapshot_;
    for (const auto &i: snapshot)
        snapshot_.push_back(boost::property_tree::ptree::value_type(i.first.name(), i.second));
    boost::property_tree::write_info(path.string(), snapshot_);
}

std::map<bunsan::pm::entry, boost::property_tree::ptree> bunsan::pm::repository::native::read_snapshot(const boost::filesystem::path &path)
{
    std::map<entry, boost::property_tree::ptree> snapshot;
    boost::property_tree::ptree snapshot_;
    boost::property_tree::read_info(path.string(), snapshot_);
    for (const auto &i: snapshot_)
    {
        const auto iter = snapshot.find(i.first);
        if (iter != snapshot.end())
            BOOST_ASSERT(iter->second == i.second);
        else
            snapshot[i.first] = i.second;
    }
    return snapshot;
}

bunsan::pm::depends bunsan::pm::repository::native::read_depends(const entry &package)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        return depends(source_resource(package, value(config::name::file::index)));
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

namespace
{
    void check_dir(const boost::filesystem::path &dir)
    {
        if (!dir.is_absolute())
            BOOST_THROW_EXCEPTION(bunsan::pm::invalid_configuration_error() <<
                                  bunsan::pm::invalid_configuration_error::path(dir) <<
                                  bunsan::pm::invalid_configuration_error::message(
                                      "You have to specify absolute path"));
        SLOG("checking "<<dir);
        if (!boost::filesystem::is_directory(dir))
        {
            if (!boost::filesystem::exists(dir))
            {
                SLOG("directory "<<dir<<" was not found");
            }
            else
            {
                SLOG(dir<<" is not a directory: starting recursive remove");
                boost::filesystem::remove_all(dir);
            }
            SLOG("trying to create "<<dir);
            boost::filesystem::create_directory(dir);
            DLOG(created);
        }
    }
}

bool bunsan::pm::repository::native::build_outdated(const entry &package, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
    boost::filesystem::path snp = package_resource(package, value(config::name::file::build_snapshot));
    boost::filesystem::path build = package_resource(package, value(config::name::file::build));
    if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
        return true;
    std::map<entry, boost::property_tree::ptree> snapshot_ = read_snapshot(snp);
    return snapshot != snapshot_;
}

bool bunsan::pm::repository::native::installation_outdated(const entry &package, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
    boost::filesystem::path snp = package_resource(package, value(config::name::file::installation_snapshot));
    boost::filesystem::path installation = package_resource(package, value(config::name::file::installation));
    if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(installation))
        return true;
    std::map<entry, boost::property_tree::ptree> snapshot_ = read_snapshot(snp);
    return snapshot != snapshot_;
}

void bunsan::pm::repository::native::check_dirs()
{
    DLOG(checking directories);
    check_dir(value(config::dir::source));
    check_dir(value(config::dir::package));
    check_dir(value(config::dir::tmp));
    DLOG(checked);
}

void bunsan::pm::repository::native::clean()
{
    DLOG(trying to clean cache);
    filesystem::reset_dir(value(config::dir::source));
    filesystem::reset_dir(value(config::dir::package));
    filesystem::reset_dir(value(config::dir::tmp));
    DLOG(cleaned);
}
