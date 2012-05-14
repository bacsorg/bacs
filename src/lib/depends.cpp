#include "bunsan/pm/depends.hpp"
#include "bunsan/pm/index.hpp"

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

// boost::property_tree::ptree convertions

bunsan::pm::depends::depends(const boost::property_tree::ptree &index)
{
    for (const auto &i: index.get_child(pm::index::package, boost::property_tree::ptree()))
        this->package.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
    for (const auto &i: index.get_child(pm::index::source::self, boost::property_tree::ptree()))
        this->source.self.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
    for (const auto &i: index.get_child(pm::index::source::import::package, boost::property_tree::ptree()))
        this->source.import.package.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
    for (const auto &i: index.get_child(pm::index::source::import::source, boost::property_tree::ptree()))
        this->source.import.source.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
}

namespace
{
    inline const std::string &to_string(const std::string &str)
    {
        return str;
    }
    inline std::string to_string(const bunsan::pm::entry &entry)
    {
        return entry.name();
    }
    template <typename Value>
    boost::property_tree::ptree multimap2ptree(const std::multimap<boost::filesystem::path, Value> &mmap)
    {
        using boost::property_tree::ptree;
        ptree pt;
        for (const auto &i: mmap)
        {
            ptree pt;
            pt.put_value(to_string(i.second));
            pt.push_back(ptree::value_type(i.first.string(), pt));
        }
        return pt;
    }
}

bunsan::pm::depends::operator boost::property_tree::ptree() const
{
    boost::property_tree::ptree deps;
    deps.put_child(index::package, multimap2ptree(this->package));
    deps.put_child(index::source::import::package, multimap2ptree(this->source.import.package));
    deps.put_child(index::source::import::source, multimap2ptree(this->source.import.source));
    deps.put_child(index::source::self, multimap2ptree(this->source.self));
    return deps;
}

