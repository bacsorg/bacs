#include <bunsan/pm/snapshot.hpp>

#include <bunsan/pm/error.hpp>

#include <bunsan/config/cast.hpp>

#include <boost/property_tree/info_parser.hpp>

bunsan::pm::snapshot bunsan::pm::read_snapshot(const boost::filesystem::path &path)
{
    try
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_info(path.string(), ptree);
        return bunsan::config::load<snapshot>(ptree);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(read_snapshot_error() <<
                              read_snapshot_error::path(path) <<
                              enable_nested_current());
    }
}

void bunsan::pm::write_snapshot(const boost::filesystem::path &path, const snapshot &snapshot_)
{
    try
    {
        boost::property_tree::write_info(
            path.string(),
            bunsan::config::save<boost::property_tree::ptree>(snapshot_)
        );
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(write_snapshot_error() <<
                              write_snapshot_error::path(path) <<
                              enable_nested_current());
    }
}
