#include <bunsan/config.hpp>

#include "extractor.hpp"

#include "cache.hpp"
#include "local_system.hpp"

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/logging/legacy.hpp>

#include <ctime>

bunsan::pm::repository::extractor::extractor(
    repository &self, const extract_config &config):
        m_self(self),
        m_config(config)
{}

void bunsan::pm::repository::extractor::extract(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        filesystem::reset_dir(destination);
        cache_().unpack_installation(package, destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_extract_error() <<
            extractor_extract_error::package(package) <<
            extractor_extract_error::destination(destination) <<
            enable_nested_current());
    }
}

void bunsan::pm::repository::extractor::install(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path meta = destination / m_config.installation.meta;
        const boost::filesystem::path snp = cache_().installation_snapshot_path(package);
        boost::filesystem::path dst = destination;
        if (m_config.installation.data)
            dst /= m_config.installation.data.get();
        bunsan::filesystem::reset_dir(destination);
        extract(package, dst);
        if (boost::filesystem::exists(meta))
        {
            BOOST_ASSERT(!m_config.installation.data);
            BOOST_THROW_EXCEPTION(installation_meta_exists_error() <<
                                  installation_meta_exists_error::meta(meta));
        }
        boost::filesystem::copy_file(
            snp,
            meta,
            boost::filesystem::copy_option::fail_if_exists
        );
        boost::filesystem::last_write_time(meta, std::time(nullptr));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_install_error() <<
            extractor_install_error::package(package) <<
            extractor_install_error::destination(destination) <<
            enable_nested_current());
    }
}

void bunsan::pm::repository::extractor::update(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path meta =
            destination / m_config.installation.meta;
        boost::optional<snapshot> snapshot_;
        if (boost::filesystem::is_regular_file(meta))
        {
            try
            {
                snapshot_ = read_snapshot(meta);
            }
            catch (std::exception &)
            {
                SLOG("warning: unable to read snapshot from " << meta <<
                     ", falling back to outdated.");
            }
        }
        if (!snapshot_ ||
            *snapshot_ != cache_().read_installation_snapshot(package))
        {
            SLOG("\"" << package << "\" installation at " << destination <<
                 " is outdated, updating...");
            install(package, destination);
        }
        else
        {
            boost::filesystem::last_write_time(meta, std::time(nullptr));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_update_error() <<
            extractor_update_error::package(package) <<
            extractor_update_error::destination(destination) <<
            enable_nested_current());
    }
}

namespace
{
    bool out_of_range(
        const std::time_t now,
        const std::time_t lifetime,
        const std::time_t last_write_time)
    {
        return now > lifetime + last_write_time ||
               last_write_time > lifetime + now;
    }
}

bool bunsan::pm::repository::extractor::need_update(
    const boost::filesystem::path &destination,
    const std::time_t &lifetime)
{
    try
    {
        SLOG("starting " << destination << " " << __func__);
        const boost::filesystem::path meta = destination / m_config.installation.meta;
        return !boost::filesystem::is_regular_file(meta) ||
               out_of_range(
                   std::time(nullptr),
                   lifetime,
                   boost::filesystem::last_write_time(meta));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_need_update_error() <<
            extractor_need_update_error::destination(destination) <<
            extractor_need_update_error::lifetime(lifetime) <<
            enable_nested_current());
    }
}

void bunsan::pm::repository::extractor::extract_source(
    const entry &package,
    const std::string &source_id,
    const boost::filesystem::path &destination)
{
    try
    {
        const tempfile tmp = local_system_().tempdir_for_build();
        cache_().unpack_source(package, source_id, tmp.path());
        merge_directories(tmp.path(), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_extract_source_error() <<
            extractor_extract_source_error::package(package) <<
            extractor_extract_source_error::source_id(source_id) <<
            extractor_extract_source_error::destination(destination) <<
            enable_nested_current());
    }
}

void bunsan::pm::repository::extractor::extract_build(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        const tempfile tmp = local_system_().tempdir_for_build();
        cache_().unpack_build(package, tmp.path());
        merge_directories(tmp.path(), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_extract_build_error() <<
            extractor_extract_build_error::package(package) <<
            extractor_extract_build_error::destination(destination) <<
            enable_nested_current());
    }
}

void bunsan::pm::repository::extractor::extract_installation(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        const tempfile tmp = local_system_().tempdir_for_build();
        cache_().unpack_installation(package, tmp.path());
        merge_directories(tmp.path(), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(
            extractor_extract_installation_error() <<
            extractor_extract_installation_error::package(package) <<
            extractor_extract_installation_error::destination(destination) <<
            enable_nested_current());
    }
}

namespace
{
    void merge_dir(
        const boost::filesystem::path &source,
        const boost::filesystem::path &destination)
    {
        using namespace bunsan;
        using namespace pm;

        try
        {
            for (boost::filesystem::directory_iterator i(source), end; i != end; ++i)
            {
                const boost::filesystem::path src = i->path();
                const boost::filesystem::path dst = destination / i->path().filename();
                if (boost::filesystem::is_directory(src) &&
                    boost::filesystem::is_directory(dst))
                {
                    merge_dir(src, dst);
                }
                else
                {
                    boost::filesystem::rename(src, dst);
                }
            }
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(
                extractor_merge_directories_error() <<
                extractor_merge_directories_error::source(source) <<
                extractor_merge_directories_error::destination(destination) <<
                enable_nested_current());
        }
    }
}

void bunsan::pm::repository::extractor::merge_directories(
    const boost::filesystem::path &source,
    const boost::filesystem::path &destination)
{
    boost::filesystem::create_directories(destination);
    merge_dir(source, destination);
}

bunsan::pm::repository::cache &bunsan::pm::repository::extractor::cache_()
{
    return m_self.cache_();
}

bunsan::pm::repository::local_system &bunsan::pm::repository::extractor::local_system_()
{
    return m_self.local_system_();
}
