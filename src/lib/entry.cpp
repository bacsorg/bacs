#include "bunsan/pm/entry.hpp"

#include <stdexcept>

#include <cassert>

#include <boost/algorithm/string/predicate.hpp>

bool bunsan::pm::entry::is_allowed_symbol(char c)
{
	return ('0'<=c && c<='9') || ('a'<=c && c<='z') || ('A'<=c && c<='Z') || c=='_' || c=='-' || c=='.';
}

bool bunsan::pm::entry::is_allowed_subpath(const std::string &subpath)
{
	return !subpath.empty() && subpath!="." && subpath!=".." && boost::algorithm::all(subpath, bunsan::pm::entry::is_allowed_symbol);
}

bunsan::pm::entry::entry(const std::string &name_): location_(name_)
{
	build(name_);
}

bunsan::pm::entry::entry(const char *name_): location_(name_)
{
	build(name_);
}

void bunsan::pm::entry::build(const std::string &name_)
{
	const std::string error("Incorrect entry name: \""+name_+"\"");
	boost::filesystem::path tmp;
	if (name_.empty())
		throw std::runtime_error(error);
	for (const auto &i: location_)
	{
		if (!is_allowed_subpath(i.generic_string()))
			throw std::runtime_error(error);
		tmp /= i.generic_string();
	}
	location_.swap(tmp);
}

bunsan::pm::entry::entry(const bunsan::pm::entry &e): location_(e.location_){}

bunsan::pm::entry &bunsan::pm::entry::operator=(const bunsan::pm::entry &e)
{
	bunsan::pm::entry ent(e);
	this->swap(ent);
	return *this;
}

bunsan::pm::entry &bunsan::pm::entry::operator=(const std::string &name_)
{
	bunsan::pm::entry ent(name_);
	this->swap(ent);
	return *this;
}

bool bunsan::pm::entry::operator==(const bunsan::pm::entry &e) const
{
	return location_==e.location_;
}

bool bunsan::pm::entry::operator<(const bunsan::pm::entry &e) const
{
	return location_<e.location_;
}

boost::filesystem::path bunsan::pm::entry::location() const
{
	return location_;
}

std::string bunsan::pm::entry::name() const
{
	return location_.generic_string();
}

void bunsan::pm::entry::swap(bunsan::pm::entry &e) throw()
{
	e.location_.swap(location_);
}

std::string bunsan::pm::entry::remote_resource(const std::string &repository, const boost::filesystem::path &name) const
{
	std::string full = repository;
	if (!full.empty() && full.back()!='/')
		full.push_back('/');
	assert(!location_.empty());
	for (const auto &i: location_)
	{
		full += i.generic_string();
			full.push_back('/');
	}
	full.resize(full.size()-1);//full.pop_back();
	for (const auto &i: name)
	{
		full.push_back('/');
		full += i.generic_string();
	}
	return full;
}

boost::filesystem::path bunsan::pm::entry::local_resource(const boost::filesystem::path &dir, const boost::filesystem::path &name) const
{
	return dir/location_/name;
}

