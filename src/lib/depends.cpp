#include "bunsan/pm/depends.hpp"

#include "bunsan/config/input_archive.hpp"
#include "bunsan/config/output_archive.hpp"

#include <algorithm>
#include <set>

#include <boost/property_tree/info_parser.hpp>

std::vector<bunsan::pm::entry> bunsan::pm::depends::all() const
{
    // TODO make it unordered
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

namespace bunsan{namespace config{namespace traits
{
    template <>
    struct is_direct_assignable<bunsan::pm::entry>: std::integral_constant<bool, true> {};
}}}

bunsan::pm::depends::operator boost::property_tree::ptree() const
{
    boost::property_tree::ptree deps;
    bunsan::config::output_archive<boost::property_tree::ptree> oa(deps);
    oa << *this;
    return deps;
}

void bunsan::pm::depends::load(const boost::property_tree::ptree &index)
{
    bunsan::config::input_archive<boost::property_tree::ptree> ia(index);
    ia >> *this;
}

void bunsan::pm::depends::load(const boost::filesystem::path &path)
{
    boost::property_tree::ptree index;
    boost::property_tree::read_info(path.string(), index);
    load(index);
}

void bunsan::pm::depends::save(const boost::filesystem::path &path) const
{
    boost::property_tree::write_info(path.string(), static_cast<boost::property_tree::ptree>(*this));
}
