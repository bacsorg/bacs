#pragma once

#include <bunsan/pm/repository.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::local_system: private boost::noncopyable
{
public:
    local_system(repository &self, const local_system_config &config);

private:
    repository &m_self;
};
