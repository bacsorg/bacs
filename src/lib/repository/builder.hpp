#pragma once

#include <bunsan/pm/repository.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::builder: private boost::noncopyable
{
public:
    builder(repository &self, const build_config &config);

    /// create empty build (for packages without sources)
    void build_empty(const entry &package);
    void build(const entry &package);
    void build_installation(const entry &package);

private:
    repository &m_self;
};
