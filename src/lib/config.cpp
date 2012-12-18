#include "bacs/archive/config.hpp"
#include "bacs/archive/error.hpp"

bunsan::utility::archiver_ptr bacs::archive::archiver_options::instance(
    const bunsan::utility::resolver &resolver) const
{
    const bunsan::utility::archiver_ptr archiver =
        bunsan::utility::factory_options<bunsan::utility::archiver>::instance(resolver);
    if (!archiver)
        BOOST_THROW_EXCEPTION(unknown_archiver_error() <<
                              unknown_archiver_error::archiver_type(type));
    return archiver;
}
