#include "repository_native.hpp"
#include "repository_config.hpp"

template <typename T>
std::multimap<boost::filesystem::path, T> bunsan::pm::repository::native::read_pairs(const entry &package, const char *child)
{
	typedef std::multimap<boost::filesystem::path, T> map_type;
	map_type map;
	boost::property_tree::ptree index;
	read_index(package, index);
	for (const auto &i: index.get_child(child, boost::property_tree::ptree()))
		map.insert(typename map_type::value_type(i.first, i.second.template get_value<std::string>()));
	return map;
}

std::multimap<boost::filesystem::path, bunsan::pm::entry> bunsan::pm::repository::native::source_imports(const entry &package)
{
	try
	{
		return read_pairs<entry>(package, child_import_source);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package source imports", e);
	}
}

std::multimap<boost::filesystem::path, bunsan::pm::entry> bunsan::pm::repository::native::package_imports(const entry &package)
{
	try
	{
		return read_pairs<entry>(package, child_import_package);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package package imports", e);
	}
}

std::multimap<boost::filesystem::path, std::string> bunsan::pm::repository::native::sources(const entry &package)
{
	try
	{
		return read_pairs<std::string>(package, child_source);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package package sources", e);
	}
}

