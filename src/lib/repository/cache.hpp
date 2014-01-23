#pragma once

#include <bunsan/pm/index.hpp>
#include <bunsan/pm/repository.hpp>
#include <bunsan/pm/snapshot.hpp>

#include <bunsan/interprocess/sync/file_lock.hpp>

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
    snapshot read_build_snapshot(const entry &package);
    snapshot read_installation_snapshot(const entry &package);

    void unpack_source(const entry &package, const boost::filesystem::path &destination);
    void unpack_build(const entry &package, const boost::filesystem::path &destination);
    void unpack_installation(const entry &package, const boost::filesystem::path &destination);

    void pack_build(const entry &package, const boost::filesystem::path &path);
    void pack_installation(const entry &package, const boost::filesystem::path &path);

    bool build_outdated(const entry &package, const snapshot &snapshot_);
    bool installation_outdated(const entry &package, const snapshot &snapshot_);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path checksum_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path index_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path source_path(const entry &package, const std::string &source_id);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path build_archive_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path build_snapshot_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path installation_archive_path(const entry &package);

    /// \note Parent directory is created if necessary.
    boost::filesystem::path installation_snapshot_path(const entry &package);

private:
    boost::filesystem::path file_path(
        const boost::filesystem::path &root,
        const entry &package,
        const boost::filesystem::path &filename);

    boost::filesystem::path source_file_path(
        const entry &package, const boost::filesystem::path &filename);

    boost::filesystem::path package_file_path(
        const entry &package, const boost::filesystem::path &filename);

    const format_config &format(); ///< \todo consider constness

private:
    local_system &local_system_();
    distributor &distributor_();

private:
    repository &m_self;
    const cache_config m_config;
    bunsan::interprocess::file_lock m_flock;
    utility::archiver_ptr m_archiver;
};
