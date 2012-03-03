#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/error.hpp"

#include <boost/unordered_set.hpp>
#include <boost/process.hpp>

bunsan::utility::resolver::resolver() {}

bunsan::utility::resolver::resolver(const boost::property_tree::ptree &config)
{
	for (const auto &i: config)
	{
		if (i.first=="absolute")
		{
			for (const auto &j: i.second)
			{
				m_absolute[j.first] = j.second.get_value<std::string>();
			}
		}
		else if (i.first=="alias")
		{
			for (const auto &j: i.second)
			{
				m_alias[j.first] = j.second.get_value<std::string>();
			}
		}
	}
}

void bunsan::utility::resolver::apply_absolute(boost::filesystem::path &name) const
{
	auto iter = m_absolute.find(name);
	if (iter!=m_absolute.end())
		name = iter->second;
}

void bunsan::utility::resolver::apply_alias(boost::filesystem::path &name) const
{
	boost::unordered_set<boost::filesystem::path> used;
	used.insert(name);
	auto iter = m_alias.find(name);
	while (iter!=m_alias.end())
	{
		name = iter->second;
		iter = m_alias.find(name);
		if (used.find(name)!=used.end())
			BOOST_THROW_EXCEPTION(bunsan::utility::error("Circular dependencies in alias is prohibited"));
		used.insert(name);
	}
}

boost::filesystem::path bunsan::utility::resolver::find_executable(const boost::filesystem::path &exe) const
{
	boost::filesystem::path ret = exe;
	apply_alias(ret);
	apply_absolute(ret);
	if (ret==ret.filename())
		return boost::process::find_executable_in_path(ret.filename().string());
	else
		return ret;
}

boost::filesystem::path bunsan::utility::resolver::find_library(const boost::filesystem::path &lib) const
{
	if (lib.is_absolute())
		return lib;
#warning unimplemented
	return lib;
}

