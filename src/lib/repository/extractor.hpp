#pragma once

#include "cache.hpp"

#include <bunsan/pm/repository.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::extractor: private boost::noncopyable
{
public:
    extractor(repository &self, const extract_config &config);

    void extract(
        const entry &package,
        const boost::filesystem::path &destination);

    void install(
        const entry &package,
        const boost::filesystem::path &destination);

    void update(
        const entry &package,
        const boost::filesystem::path &destination);

    bool need_update(
        const boost::filesystem::path &destination,
        const std::time_t &lifetime);

    /*!
     * \note Will merge directories but will fail on file collisions
     * \warning Will work only on the local_system_().tempdir_for_build()'s filesystem
     */
    void extract_source(
        const entry &package,
        const std::string &source_id,
        const boost::filesystem::path &destination);

    void extract_build(
        const entry &package,
        const boost::filesystem::path &destination);

    void extract_installation(
        const entry &package,
        const boost::filesystem::path &destination);

private:
    cache &cache_();
    local_system &local_system_();

private:
    repository &m_self;
    extract_config m_config;
};
