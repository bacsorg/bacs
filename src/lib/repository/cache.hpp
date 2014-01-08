#pragma once

#include <bunsan/pm/index.hpp>
#include <bunsan/pm/repository.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::cache: private boost::noncopyable
{
public:
    cache(repository &self, const cache_config &config);

    typedef bool lock_guard; // FIXME
    lock_guard lock();

    void validate_and_repair();

    void clean();

    index read_index(const entry &package);
    snapshot_entry read_checksum(const entry &package);

    bool installation_outdated(const entry &package, const snapshot &snapshot_);
    bool build_outdated(const entry &package, const snapshot &snapshot_);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path checksum_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path index_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path source_path(const entry &package, const std::string &source_id);

private:
    repository &m_self;
};
