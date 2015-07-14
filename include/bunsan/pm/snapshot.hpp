#pragma once

#include <bunsan/pm/entry.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <map>
#include <string>

namespace bunsan{namespace pm
{
    using source_name_type = std::string;
    using checksum_type = std::string;
    using snapshot_entry = std::map<source_name_type, checksum_type>;
    using snapshot = std::map<entry, snapshot_entry>;

    snapshot_entry read_checksum(const boost::filesystem::path &path);
    void write_checksum(const boost::filesystem::path &path, const snapshot_entry &checksum);

    snapshot read_snapshot(const boost::filesystem::path &path);
    void write_snapshot(const boost::filesystem::path &path, const snapshot &snapshot_);
}}
