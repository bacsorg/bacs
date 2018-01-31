#include <bunsan/error.hpp>

namespace bunsan {

error::error() {}

error::error(const std::string &message_) { (*this) << message(message_); }

const char *error::what() const noexcept {
  return boost::diagnostic_information_what(*this);
}

void enable_stacktrace::operator()(const boost::exception &e) const {
  if (!boost::get_error_info<error::stacktrace>(e))
    e << error::stacktrace(runtime::stacktrace::get(m_skip + 1));
}

void enable_nested::operator()(const boost::exception &e) const {
  e << error::nested_exception(m_ptr);
}

void enable_nested_current::operator()(const boost::exception &e) const {
  e << enable_nested(boost::current_exception());
}

}  // namespace bunsan
