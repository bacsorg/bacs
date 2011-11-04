#include "repository_native.hpp"
#include "bunsan/pm/index.hpp"
#include "bunsan/pm/config.hpp"

#include <map>
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "bunsan/executor.hpp"

#include "bunsan/pm/checksum.hpp"

void bunsan::pm::repository::native::create(const boost::filesystem::path &source, bool strip)
{
	boost::filesystem::path index_name = source/value(config::name::file::index);
	boost::filesystem::path checksum_name = source/value(config::name::file::checksum);
	std::map<std::string, std::string> checksum;
	// we need to save index checksum
	checksum[value(config::name::file::index)] = bunsan::pm::checksum(index_name);
	std::set<std::string> keep;
	// we will keep index and checksum files
	keep.insert(value(config::name::file::index));
	keep.insert(value(config::name::file::checksum));
	bunsan::executor creator(config.get_child(config::command::create));
	creator.current_path(source);// FIXME encapsulation fault
	boost::property_tree::ptree index;
	boost::property_tree::read_info(index_name.generic_string(), index);
	for (const auto &i: index.get_child(index::source::self))
	{
		std::string src_name = i.second.get_value<std::string>();
		std::string src_value = src_name+value(config::suffix::src);
		boost::filesystem::path src = source/src_name;
		boost::filesystem::path dst = boost::filesystem::absolute(source/(src_value));
		if (!boost::filesystem::exists(source/src_name))
			throw std::runtime_error("Source does not exists: \""+src_name+"\"");
		creator(src_name, dst);
		checksum[src_name] = bunsan::pm::checksum(source/src_value);
		keep.insert(src_value);// we will also keep all source tarballs
	}
	{
		boost::property_tree::ptree checksum_;
		for (const auto &i: checksum)
			checksum_.put(boost::property_tree::ptree::path_type(i.first, '\0'), i.second);
		boost::property_tree::write_info(checksum_name.generic_string(), checksum_);
	}
	if (strip)
		for (boost::filesystem::directory_iterator i(source); i!=boost::filesystem::directory_iterator(); ++i)
		{
			if (keep.find(i->path().filename().generic_string())==keep.end())
			{
				SLOG("Removing excess file from source package: "<<i->path());
				boost::filesystem::remove_all(*i);
			}
		}
}

