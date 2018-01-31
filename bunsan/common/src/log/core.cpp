#include <bunsan/log/core.hpp>

#include <bunsan/log/expressions/scope.hpp>
#include <bunsan/log/trivial.hpp>

#include <boost/log/core.hpp>
#include <boost/phoenix/bind.hpp>

namespace bunsan {
namespace log {

bool set_enabled(bool enabled) {
  return boost::log::core::get()->set_logging_enabled(enabled);
}

void remove_default_sink() { trivial::global::remove_default_sink(); }

void set_minimum_severity(const severity minimum_severity) {
  const auto filter = [minimum_severity](
      boost::log::value_ref<severity, expressions::tag::severity> const &
          severity) { return !severity || severity >= minimum_severity; };
  boost::log::core::get()->set_filter(
      boost::phoenix::bind(filter, expressions::severity.or_none()));
}

}  // namespace log
}  // namespace bunsan
