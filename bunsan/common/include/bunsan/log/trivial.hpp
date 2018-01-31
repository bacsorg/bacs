#pragma once

#include <bunsan/log/keywords/scope.hpp>
#include <bunsan/log/severity.hpp>
#include <bunsan/log/sources/scope.hpp>
#include <bunsan/log/workaround/all.hpp>

#include <boost/current_function.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace bunsan {
namespace log {
namespace trivial {

template <typename Level = int>
class logger
    : public boost::log::sources::basic_composite_logger<
          char, logger<Level>,
          boost::log::sources::multi_thread_model<boost::shared_mutex>,
          boost::log::sources::features<boost::log::sources::severity<Level>,
                                        sources::scope>> {};

struct global {
  static logger<severity> &get();
  static void remove_default_sink();
};

}  // namespace trivial
}  // namespace log
}  // namespace bunsan

#define BUNSAN_LOG_TRIVIAL(SEV)                                          \
  BOOST_LOG_STREAM_WITH_PARAMS(                                          \
      ::bunsan::log::trivial::global::get(),                             \
      (::boost::log::keywords::severity = ::bunsan::log::severity::SEV)( \
          ::bunsan::log::keywords::file =                                \
              __FILE__)(::bunsan::log::keywords::line = __LINE__)(       \
          ::bunsan::log::keywords::function = __func__)(                 \
          ::bunsan::log::keywords::pretty_function = BOOST_CURRENT_FUNCTION))

#define BUNSAN_LOG_TRACE BUNSAN_LOG_TRIVIAL(trace)
#define BUNSAN_LOG_DEBUG BUNSAN_LOG_TRIVIAL(debug)
#define BUNSAN_LOG_INFO BUNSAN_LOG_TRIVIAL(info)
#define BUNSAN_LOG_WARNING BUNSAN_LOG_TRIVIAL(warning)
#define BUNSAN_LOG_ERROR BUNSAN_LOG_TRIVIAL(error)
#define BUNSAN_LOG_FATAL BUNSAN_LOG_TRIVIAL(fatal)
