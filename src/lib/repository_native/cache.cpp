#include "../repository_native.hpp"

#include <bunsan/logging/legacy.hpp>

#include <bunsan/filesystem/operations.hpp>

namespace
{
    void check_dir(const boost::filesystem::path &dir)
    {
        using namespace bunsan;
        using namespace pm;
        try
        {
            if (!dir.is_absolute())
                BOOST_THROW_EXCEPTION(invalid_configuration_error() <<
                                      invalid_configuration_error::path(dir) <<
                                      invalid_configuration_error::message(
                                          "You have to specify absolute path"));
            SLOG("checking " << dir);
            if (!boost::filesystem::is_directory(dir))
            {
                if (!boost::filesystem::exists(dir))
                {
                    SLOG("directory " << dir << " was not found");
                }
                else
                {
                    SLOG(dir << " is not a directory: starting recursive remove");
                    boost::filesystem::remove_all(dir);
                }
                if (boost::filesystem::create_directory(dir))
                    SLOG("created missing " << dir << " directory");
            }
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(native_check_dir_error() <<
                                  native_check_dir_error::path(dir) <<
                                  enable_nested_current());
        }
    }
}

void bunsan::pm::repository::native::check_cache()
{
    try
    {
        check_dirs();
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_check_cache_error() <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::check_dirs()
{
    try
    {
        DLOG(checking directories);
        check_dir(m_config.dir.source);
        check_dir(m_config.dir.package);
        check_dir(m_config.dir.tmp);
        DLOG(directories checked);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_check_dirs_error() <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::clean()
{
    try
    {
        DLOG(trying to clean cache);
        filesystem::reset_dir(m_config.dir.source);
        filesystem::reset_dir(m_config.dir.package);
        filesystem::reset_dir(m_config.dir.tmp);
        DLOG(cache cleaned);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_clean_error() <<
                              enable_nested_current());
    }
}
