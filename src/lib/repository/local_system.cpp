#include "local_system.hpp"

bunsan::pm::repository::local_system::local_system(
    repository &self, const local_system_config &config):
        m_self(self),
        m_config(config),
        m_resolver(m_config.resolver)
{}

const bunsan::utility::resolver &bunsan::pm::repository::local_system::resolver()
{
    return m_resolver;
}

bunsan::tempfile bunsan::pm::repository::local_system::tempdir_for_build()
{
    return tempfile::directory_in_directory(m_config.build_dir);
}

bunsan::tempfile bunsan::pm::repository::local_system::small_tempfile()
{
    return m_config.tmp_file ?
        tempfile::regular_file_from_model(*m_config.tmp_file) :
        tempfile::regular_file_in_tempdir();
}
