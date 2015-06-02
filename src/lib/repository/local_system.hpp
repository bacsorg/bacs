#pragma once

#include <bunsan/pm/repository.hpp>

#include <bunsan/tempfile.hpp>
#include <bunsan/utility/custom_resolver.hpp>

#include <boost/noncopyable.hpp>

class bunsan::pm::repository::local_system: private boost::noncopyable
{
public:
    local_system(repository &self, const local_system_config &config);

    utility::resolver &resolver();

    /// Empty dir for possibly large files.
    tempfile tempdir_for_build();

    /// Empty file.
    tempfile small_tempfile();

private:
    repository &m_self;
    local_system_config m_config;
    utility::custom_resolver m_resolver;
};
