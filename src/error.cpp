#include <bunsan/error.hpp>

bunsan::error::error()
{
    (*this) << enable_stacktrace();
}

bunsan::error::error(const std::string &message_)
{
    (*this) << enable_stacktrace();
    (*this) << message(message_);
}

const char *bunsan::error::what() const noexcept
{
    return boost::diagnostic_information_what(*this);
}

void bunsan::enable_stacktrace::operator()(const boost::exception &e) const
{
    if (!boost::get_error_info<error::stacktrace>(e))
        e << error::stacktrace(runtime::stacktrace::get(m_skip + 1));
}

void bunsan::enable_nested::operator()(const boost::exception &e) const
{
    e << error::nested_exception(m_ptr);
}

void bunsan::enable_nested_current::operator()(const boost::exception &e) const
{
    e << enable_nested(boost::current_exception());
}
