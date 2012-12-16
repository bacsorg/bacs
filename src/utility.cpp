#include "bunsan/utility/utility.hpp"

#include <boost/assert.hpp>

bunsan::utility::unknown_option_error::unknown_option_error(const std::string &option)
{
    (*this) << error_option(option);
}

std::string bunsan::utility::unknown_option_error::option() const
{
    const std::string *option_ = boost::get_error_info<error_option>(*this);
    BOOST_ASSERT_MSG(option_, "unable to retrieve option that was initialized in constructor");
    return *option_;
}

bunsan::utility::utility::~utility() {}

void bunsan::utility::utility::setup(const utility::config_type &config)
{
    for (const auto &i: config)
        setarg(i.first, i.second.get_value<std::string>());
}

void bunsan::utility::utility::setarg(const std::string &key, const std::string &/*value*/)
{
    BOOST_THROW_EXCEPTION(unknown_option_error(key));
}

void bunsan::utility::utility::setarg_any(const std::string &key, const boost::any &/*value*/)
{
    BOOST_THROW_EXCEPTION(unknown_option_error(key));
}
