#include "repository_native.hpp"

#include "bunsan/pm/config.hpp"
#include "bunsan/pm/index.hpp"
#include "bunsan/pm/snapshot.hpp"

#include "bunsan/pm/checksum.hpp"

#include "bunsan/logging/legacy.hpp"

#include <unordered_set>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

void bunsan::pm::repository::native::create(const boost::filesystem::path &source, bool strip)
{
    const boost::filesystem::path index_name = source / m_config.name.file.index;
    const boost::filesystem::path checksum_name = source / m_config.name.file.checksum;
    snapshot_entry checksum;
    // we need to save index checksum
    checksum[m_config.name.file.index] = bunsan::pm::checksum(index_name);
    std::unordered_set<std::string> to_remove;
    index index_;
    index_.load(index_name);
    for (const std::string &src_name: index_.sources())
    {
        const std::string src_value = src_name + m_config.suffix.source_archive;
        const boost::filesystem::path src = source / src_name;
        const boost::filesystem::path dst = boost::filesystem::absolute(source / src_value);
        if (!boost::filesystem::exists(source / src_name))
            BOOST_THROW_EXCEPTION(error("Source does not exist") << error::path(src_name)); // TODO better error name
        source_archiver->pack(dst, source / src_name);
        checksum[src_name] = bunsan::pm::checksum(source / src_value);
        to_remove.insert(src_name); // we will remove all sources
    }
    {
        boost::property_tree::ptree checksum_;
        for (const auto &i: checksum)
            checksum_.put(boost::property_tree::ptree::path_type(i.first, '\0'), i.second);
        boost::property_tree::write_info(checksum_name.string(), checksum_);
    }
    // we will remove all files at the end to provide exception guarantee that we will not remove anything accidentally
    if (strip)
        for (const std::string &i: to_remove)
        {
            boost::filesystem::path path = source / i;
            SLOG("Removing excess file from source package: " << path);
            boost::filesystem::remove_all(path);
        }
}

void bunsan::pm::repository::native::create_recursively(const boost::filesystem::path &root, bool strip)
{
    std::unordered_set<std::string> ignore;
    const boost::filesystem::path index_path = root / m_config.name.file.index;
    if (boost::filesystem::is_regular_file(index_path))
    {
        SLOG("Found index file at " << root << ", trying to create source package...");
        ignore.insert(m_config.name.file.index);
        create(root, strip);
        index index_;
        index_.load(index_path);
        const std::unordered_set<std::string> sources = index_.sources();
        ignore.insert(sources.begin(), sources.end());
    }
    for (boost::filesystem::directory_iterator i(root), end; i != end; ++i)
    {
        if (boost::filesystem::is_directory(*i) && ignore.find(i->path().filename().string()) == ignore.end())
            create_recursively(i->path(), strip);
    }
}
