#include "repository_native.hpp"

#include "bunsan/pm/config.hpp"
#include "bunsan/pm/index.hpp"

#include "bunsan/pm/checksum.hpp"

#include "bunsan/logging/legacy.hpp"

#include <map>
#include <set>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

void bunsan::pm::repository::native::create(const boost::filesystem::path &source, bool strip)
{
    const boost::filesystem::path index_name = source / m_config.name.file.index;
    const boost::filesystem::path checksum_name = source / m_config.name.file.checksum;
    std::map<std::string, std::string> checksum;
    // we need to save index checksum
    checksum[m_config.name.file.index] = bunsan::pm::checksum(index_name);
    std::set<std::string> to_remove;
    index index_;
    index_.load(index_name);
    for (const auto &i: index_.source.self)
    {
        const std::string src_name = i.second;
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
        for (const auto &i: to_remove)
        {
            boost::filesystem::path path = source / i;
            SLOG("Removing excess file from source package: " << path);
            boost::filesystem::remove_all(path);
        }
}
