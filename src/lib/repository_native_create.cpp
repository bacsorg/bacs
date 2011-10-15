#include "repository_native.hpp"
#include "repository_config.hpp"

#include <map>
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "bunsan/executor.hpp"

#include "bunsan/pm/checksum.hpp"

void bunsan::pm::repository::native::create(const boost::filesystem::path &source, bool strip)
{
	boost::filesystem::path index_name = source/value(name_file_index);
	boost::filesystem::path checksum_name = source/value(name_file_checksum);
	std::map<std::string, std::string> checksum;
	// we need to save index checksum
	checksum[value(name_file_index)] = bunsan::pm::checksum(index_name);
	std::set<std::string> keep;
	// we will keep index and checksum files
	keep.insert(value(name_file_index));
	keep.insert(value(name_file_checksum));
	bunsan::executor creator(config.get_child(command_create));
	creator.current_path(source);// FIXME encapsulation fault
	boost::property_tree::ptree index;
	boost::property_tree::read_info(index_name.native(), index);
	for (const auto &i: index.get_child(child_source))
	{
		std::string src_name = i.second.get_value<std::string>();
		std::string src_value = src_name+value(suffix_src);
		boost::filesystem::path src = source/src_name;
		boost::filesystem::path dst = source/(src_value);
		creator(src_name, dst);
		checksum[src_name] = bunsan::pm::checksum(src_value);
		keep.insert(src_value);// we will also keep all source tarballs
	}
	{
		boost::property_tree::ptree checksum_;
		for (const auto &i: checksum)
			checksum_.put(boost::property_tree::ptree::path_type(i.first, '\0'), i.second);
		boost::property_tree::write_info(checksum_name.native(), checksum_);
	}
	if (strip)
		for (boost::filesystem::directory_iterator i(source); i!=boost::filesystem::directory_iterator(); ++i)
		{
			if (keep.find(i->path().filename().native())==keep.end())
			{
				SLOG("Removing excess file from source package: "<<i->path());
				boost::filesystem::remove_all(*i);
			}
		}
}

