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
    if (!(m_archiver = m_config.format.archiver.instance_optional(local_system_().resolver())))
    {
        BOOST_THROW_EXCEPTION(
            invalid_configuration_remote_archiver_error() <<
            invalid_configuration_remote_archiver_error::utility_type(
                m_config.format.archiver.type));
    }
    if (!(m_fetcher = m_config.fetcher.instance_optional(local_system_().resolver())))
    {
        BOOST_THROW_EXCEPTION(
            invalid_configuration_fetcher_error() <<
            invalid_configuration_fetcher_error::utility_type(m_config.fetcher.type));
    }
}

void bunsan::pm::repository::distributor::create(
    const boost::filesystem::path &source, bool strip)
{
    try
    {
        const boost::filesystem::path index_name =
            source / m_config.format.name.get_index();
        const boost::filesystem::path checksum_name =
            source / m_config.format.name.get_checksum();
        snapshot_entry checksum;
        // we need to save index checksum
        checksum[m_config.format.name.get_index()] = bunsan::pm::checksum(index_name);
        std::unordered_set<std::string> to_remove;
        index index_;
        index_.load(index_name);
        for (const std::string &src_name: index_.sources())
        {
            const std::string src_value =
                src_name + m_config.format.name.suffix.archive;
            const boost::filesystem::path src = source / src_name;
            const boost::filesystem::path dst =
                boost::filesystem::absolute(source / src_value);
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
        const boost::filesystem::path index_path = root / m_config.format.name.get_index();
        if (boost::filesystem::is_regular_file(index_path))
        {
            SLOG("Found index file at " << root << ", trying to create source package...");
            ignore.insert(m_config.format.name.get_index());
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

bunsan::pm::repository::local_system &bunsan::pm::repository::distributor::local_system_()
{
    return m_self.local_system_();
}

bunsan::pm::repository::cache &bunsan::pm::repository::distributor::cache_()
{
    return m_self.cache_();
}
