#include "bunsan/pm/depends.hpp"

#include <algorithm>
#include <set>

std::vector<bunsan::pm::entry> bunsan::pm::depends::all() const
{
	std::set<entry> all_;
	for (const auto &i: package)
		all_.insert(i.second);
	for (const auto &i: source.import.package)
		all_.insert(i.second);
	for (const auto &i: source.import.source)
		all_.insert(i.second);
	std::vector<entry> all__(all_.size());
	std::copy(all_.begin(), all_.end(), all__.begin());
	return all__;
}

