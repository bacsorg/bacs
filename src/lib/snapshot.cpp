#include <bunsan/pm/snapshot.hpp>

#include <bunsan/pm/error.hpp>

#include <bunsan/config/cast.hpp>

#include <boost/property_tree/info_parser.hpp>

#define BUNSAN_PM_SERIALIZE(NAME, TYPE) \
    bunsan::pm::TYPE bunsan::pm::read_##NAME(const boost::filesystem::path &path) \
    { \
        try \
        { \
            boost::property_tree::ptree ptree; \
            boost::property_tree::read_info(path.string(), ptree); \
            return bunsan::config::load<TYPE>(ptree); \
        } \
        catch (std::exception &) \
        { \
            BOOST_THROW_EXCEPTION(read_##NAME##_error() << \
                                  read_##NAME##_error::path(path) << \
                                  enable_nested_current()); \
        } \
    } \
    void bunsan::pm::write_##NAME(const boost::filesystem::path &path, const TYPE &NAME##_) \
    { \
        try \
        { \
            boost::property_tree::write_info( \
                path.string(), \
                bunsan::config::save<boost::property_tree::ptree>(NAME##_) \
            ); \
        } \
        catch (std::exception &) \
        { \
            BOOST_THROW_EXCEPTION(write_##NAME##_error() << \
                                  write_##NAME##_error::path(path) << \
                                  enable_nested_current()); \
        } \
    }

BUNSAN_PM_SERIALIZE(checksum, snapshot_entry)
BUNSAN_PM_SERIALIZE(snapshot, snapshot)
