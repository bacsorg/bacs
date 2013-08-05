#include <bunsan/pm/index.hpp>

#include <bunsan/config/input_archive.hpp>
#include <bunsan/config/output_archive.hpp>

#include <algorithm>

#include <boost/property_tree/info_parser.hpp>

bool bunsan::pm::index::stage::empty() const
{
    return self.empty() && import.package.empty() && import.source.empty();
}

std::set<bunsan::pm::entry> bunsan::pm::index::stage::all() const
{
    std::set<entry> all_;
    for (const auto &i: import.package)
        all_.insert(i.second);
    for (const auto &i: import.source)
        all_.insert(i.second);
    return all_;
}

std::set<bunsan::pm::entry> bunsan::pm::index::all() const
{
    std::set<entry> all_ = source.all();
    const std::set<entry> all_package = package.all();
    all_.insert(all_package.begin(), all_package.end());
    return all_;
}

std::unordered_set<std::string> bunsan::pm::index::sources() const
{
    std::unordered_set<std::string> sources_;
    for (const auto &i: source.self)
        sources_.insert(i.second);
    for (const auto &i: package.self)
        sources_.insert(i.second);
    return sources_;
}

namespace bunsan{namespace config{namespace traits
{
    template <>
    struct is_direct_assignable<bunsan::pm::entry>: std::integral_constant<bool, true> {};
}}}

bunsan::pm::index::operator boost::property_tree::ptree() const
{
    boost::property_tree::ptree deps;
    bunsan::config::output_archive<boost::property_tree::ptree> oa(deps);
    oa << *this;
    return deps;
}

void bunsan::pm::index::load(const boost::property_tree::ptree &ptree)
{
    bunsan::config::input_archive<boost::property_tree::ptree> ia(ptree);
    ia >> *this;
}

void bunsan::pm::index::load(const boost::filesystem::path &path)
{
    boost::property_tree::ptree ptree;
    boost::property_tree::read_info(path.string(), ptree);
    load(ptree);
}

void bunsan::pm::index::save(const boost::filesystem::path &path) const
{
    boost::property_tree::write_info(path.string(), static_cast<boost::property_tree::ptree>(*this));
}
