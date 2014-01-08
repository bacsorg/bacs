#include <bunsan/config.hpp>

#include "distributor.hpp"

#include "cache.hpp"
#include "local_system.hpp"

#include <bunsan/pm/checksum.hpp>
#include <bunsan/pm/config.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/pm/snapshot.hpp>

#include <bunsan/logging/legacy.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <unordered_set>

bunsan::pm::repository::distributor::distributor(
    repository &self, const remote_config &config):
        m_self(self),
        m_config(config)
{
    if (!(m_archiver = format().archiver.instance_optional(local_system_().resolver())))
    {
        BOOST_THROW_EXCEPTION(
            invalid_configuration_remote_archiver_error() <<
            invalid_configuration_remote_archiver_error::utility_type(format().archiver.type));
    }
    if (!(m_fetcher = m_config.fetcher.instance_optional(local_system_().resolver())))
    {
        BOOST_THROW_EXCEPTION(
            invalid_configuration_fetcher_error() <<
            invalid_configuration_fetcher_error::utility_type(m_config.fetcher.type));
    }
}

const bunsan::pm::format_config &bunsan::pm::repository::distributor::format() const
{
    return m_config.format;
}

void bunsan::pm::repository::distributor::create(
    const boost::filesystem::path &source, bool strip)
{
    try
    {
        const boost::filesystem::path index_name = source / format().name.get_index();
        const boost::filesystem::path checksum_name = source / format().name.get_checksum();
        snapshot_entry checksum;
        // we need to save index checksum
        checksum[format().name.get_index()] = bunsan::pm::checksum(index_name);
        std::unordered_set<std::string> to_remove;
        index index_;
        index_.load(index_name);
        for (const std::string &src_name: index_.sources())
        {
            const std::string src_value = src_name + format().name.suffix.archive;
            const boost::filesystem::path src = source / src_name;
            const boost::filesystem::path dst = boost::filesystem::absolute(source / src_value);
            if (!boost::filesystem::exists(src))
                BOOST_THROW_EXCEPTION(source_does_not_exist_error() <<
                                      source_does_not_exist_error::source(src_name) <<
                                      source_does_not_exist_error::path(src));
            m_archiver->pack(dst, src);
            checksum[src_name] = bunsan::pm::checksum(source / src_value);
            to_remove.insert(src_name); // we will remove all sources
        }
        {
            boost::property_tree::ptree checksum_;
            for (const auto &i: checksum)
                checksum_.put(boost::property_tree::ptree::path_type(i.first, '\0'), i.second);
            boost::property_tree::write_info(checksum_name.string(), checksum_);
        }
        // we will remove all files at the end to provide exception guarantee
        // that we will not remove anything accidentally
        if (strip)
            for (const std::string &i: to_remove)
            {
                boost::filesystem::path path = source / i;
                SLOG("Removing excess file from source package: " << path);
                boost::filesystem::remove_all(path);
            }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(distributor_create_error() <<
                              distributor_create_error::source(source) <<
                              distributor_create_error::strip(strip) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::distributor::create_recursively(
    const boost::filesystem::path &root, bool strip)
{
    try
    {
        std::unordered_set<std::string> ignore;
        const boost::filesystem::path index_path = root / format().name.get_index();
        if (boost::filesystem::is_regular_file(index_path))
        {
            SLOG("Found index file at " << root << ", trying to create source package...");
            ignore.insert(format().name.get_index());
            create(root, strip);
            index index_;
            index_.load(index_path);
            const std::unordered_set<std::string> sources = index_.sources();
            ignore.insert(sources.begin(), sources.end());
        }
        for (boost::filesystem::directory_iterator i(root), end; i != end; ++i)
        {
            if (boost::filesystem::is_directory(*i) &&
                ignore.find(i->path().filename().string()) == ignore.end())
            {
                create_recursively(i->path(), strip);
            }
        }
    }
    catch (distributor_create_recursively_error &)
    {
        throw;
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(distributor_create_recursively_error() <<
                              distributor_create_recursively_error::root(root) <<
                              distributor_create_recursively_error::strip(strip) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::distributor::update_meta(const entry &package)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const tempfile checksum_tmp = local_system_().small_tempfile();
        try
        {
            m_fetcher->fetch(checksum_url(package), checksum_tmp.path());
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(
                distributor_update_meta_no_package_error() <<
                distributor_update_meta_no_package_error::path(checksum_tmp.path()) <<
                distributor_update_meta_no_package_error::message("Unable to fetch checksum") <<
                enable_nested_current());
        }
        boost::filesystem::copy_file(checksum_tmp.path(), cache_().checksum_path(package),
                                     boost::filesystem::copy_option::overwrite_if_exists);
        update_file(
            index_url(package),
            cache_().index_path(package),
            // \pre index is treated like regular source
            cache_().read_checksum(package).at(format().name.get_index()));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(distributor_update_meta_error() <<
                              distributor_update_meta_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::distributor::update_sources(const entry &package)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        for (const std::string &src_name: cache_().read_index(package).sources())
        {
            update_file(
                source_url(package, src_name),
                cache_().source_path(package, src_name),
                cache_().read_checksum(package).at(src_name));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(distributor_update_sources_error() <<
                              distributor_update_sources_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::distributor::update_file(
    const std::string &url,
    const boost::filesystem::path &file,
    const std::string &checksum)
{
    const auto outdated =
        [](const boost::filesystem::path &file, const std::string &checksum)
        {
            return !boost::filesystem::exists(file) || bunsan::pm::checksum(file) != checksum;
        };
    if (outdated(file, checksum))
    {
        m_fetcher->fetch(url, file);
        if (outdated(file, checksum))
            BOOST_THROW_EXCEPTION(
                distributor_update_meta_inconsistent_checksum_error() <<
                distributor_update_meta_inconsistent_checksum_error::url(url) <<
                distributor_update_meta_inconsistent_checksum_error::path(file));
    }
}

std::string bunsan::pm::repository::distributor::index_url(const entry &package) const
{
    return url(package, format().name.get_index());
}

std::string bunsan::pm::repository::distributor::checksum_url(const entry &package) const
{
    return url(package, format().name.get_checksum());
}

std::string bunsan::pm::repository::distributor::source_url(
    const entry &package, const std::string &source_id) const
{
    return url(package, source_id + format().name.suffix.archive);
}

std::string bunsan::pm::repository::distributor::url(
    const entry &package, const boost::filesystem::path &name) const
{
    return package.remote_resource(m_config.url, name);
}

bunsan::pm::repository::local_system &bunsan::pm::repository::distributor::local_system_()
{
    return m_self.local_system_();
}

bunsan::pm::repository::cache &bunsan::pm::repository::distributor::cache_()
{
    return m_self.cache_();
}
