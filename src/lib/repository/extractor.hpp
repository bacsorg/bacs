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

private:
    void extract(
        const entry &package,
        const boost::filesystem::path &destination,
        bool reset);
};
