#include "local_system.hpp"

#include <boost/filesystem/operations.hpp>

bunsan::pm::repository::local_system::local_system(
    repository &self, const local_system_config &config):
        m_self(self),
        m_config(config),
        m_resolver(m_config.resolver)
{}

bunsan::utility::resolver &bunsan::pm::repository::local_system::resolver()
{
    return m_resolver;
}

bunsan::tempfile bunsan::pm::repository::local_system::tempdir_for_build()
{
    if (!m_config.build_dir.is_absolute())
        BOOST_THROW_EXCEPTION(
            invalid_configuration_relative_path_error() <<
            invalid_configuration_relative_path_error::path(m_config.build_dir));
    boost::filesystem::create_directories(m_config.build_dir);
    return tempfile::directory_in_directory(m_config.build_dir);
}

bunsan::tempfile bunsan::pm::repository::local_system::small_tempfile()
{
    return m_config.tmp_file ?
        tempfile::regular_file_from_model(*m_config.tmp_file) :
        tempfile::regular_file_in_tempdir();
}
