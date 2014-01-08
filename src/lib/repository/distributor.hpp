#pragma once

#include <bunsan/pm/repository.hpp>

#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/fetcher.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::distributor: private boost::noncopyable
{
public:
    distributor(repository &self, const remote_config &config);

    void create(const boost::filesystem::path &source, bool strip);
    void create_recursively(const boost::filesystem::path &root, bool strip);

    void update_meta(const entry &package);
    void fetch_source(const entry &package);

private:
    local_system &local_system_();
    cache &cache_();

private:
    repository &m_self;
    const remote_config m_config;
    utility::archiver_ptr m_archiver;
    utility::fetcher_ptr m_fetcher;
};
