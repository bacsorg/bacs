#ifndef BUNSAN_PM_DEPENDS_HPP
#define BUNSAN_PM_DEPENDS_HPP

#include <map>

#include <boost/filesystem/path.hpp>

#include "bunsan/pm/entry.hpp"

namespace bunsan{namespace pm
{
	struct depends
	{
		depends()=default;
		depends(const depends &)=default;
		depends(depends &&)=default;
		depends &operator=(const depends &)=default;
		depends &operator=(depends &&)=default;
		std::multimap<boost::filesystem::path, entry> package;
		struct
		{
			struct
			{
				std::multimap<boost::filesystem::path, entry> source, package;
			} import;
			std::multimap<boost::filesystem::path, std::string> self;
		} source;
		std::vector<entry> all() const;
		// boost::property_tree::ptree convertions
		explicit depends(const boost::property_tree::ptree &index);
		explicit operator boost::property_tree::ptree() const;
	};
}}

#endif //BUNSAN_PM_DEPENDS_HPP

