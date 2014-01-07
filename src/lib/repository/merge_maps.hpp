#pragma once

#include <map>

namespace bunsan{namespace pm
{
    /// merges first and second map into the first one checking contradictions
    template <typename Key, typename Value>
    void merge_maps(std::map<Key, Value> &a, const std::map<Key, Value> &b)
    {
        for (const auto &i: b)
        {
            auto iter = a.find(i.first);
            if (iter != a.end())
                BOOST_ASSERT(iter->second == i.second);
            else
                a[i.first] = i.second;
        }
    }
}}
