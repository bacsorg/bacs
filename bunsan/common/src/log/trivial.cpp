#include <bunsan/log/trivial.hpp>

#include <bunsan/log/expressions/scope.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <iostream>
#include <mutex>

namespace bunsan {
namespace log {
namespace trivial {

namespace blog = boost::log;

static void initialize_attributes() {
  static std::once_flag once;
  std::call_once(once, blog::add_common_attributes);
}

static std::once_flag default_sink_create_flag;
static std::once_flag default_sink_remove_flag;
static boost::shared_ptr<blog::sinks::sink> default_sink;

static void initialize_default_sink() {
  namespace keywords = blog::keywords;
  namespace attrs = blog::attributes;
  namespace expr = blog::expressions;

  default_sink =
      blog::add_console_log(
          std::cerr,
          keywords::format =
              (expr::stream
               << "[" << expr::format_date_time<boost::posix_time::ptime>(
                             "TimeStamp", "%d %b %Y %H:%M:%S.%f") << " <"
               << log::expressions::severity << "> " << log::expressions::file
               << "(" << log::expressions::function << ")"
               << ":" << log::expressions::line << "] " << expr::smessage));
}

logger<severity> &global::get() {
  static logger<severity> log;
  initialize_attributes();
  std::call_once(default_sink_create_flag, initialize_default_sink);
  return log;
}

void global::remove_default_sink() {
  std::call_once(default_sink_create_flag, [] {});
  std::call_once(default_sink_remove_flag, [] {
    if (default_sink) {
      blog::core::get()->remove_sink(default_sink);
      default_sink.reset();
    }
  });
}

}  // namespace trivial
}  // namespace log
}  // namespace bunsan
