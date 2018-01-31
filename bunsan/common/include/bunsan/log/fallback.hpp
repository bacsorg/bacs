#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <ostream>
#include <utility>

namespace bunsan {
namespace log {
namespace fallback {

class logger {
 public:
  explicit logger(std::ostream &out) : m_out(out) {}

  ~logger() { m_out << std::endl; }

  template <typename T>
  logger &operator<<(T &&obj) {
    m_out << std::forward<T>(obj);
    return *this;
  }

 private:
  std::ostream &m_out;
};

}  // namespace fallback
}  // namespace log
}  // namespace bunsan

#define BUNSAN_LOG_SEV_INTO(SEV, STREAM)                                 \
  ::bunsan::log::fallback::logger(STREAM)                                \
      << "[" << ::boost::posix_time::second_clock::local_time() << " <"  \
      << #SEV << "> " << __FILE__ << "(" << __func__ << "):" << __LINE__ \
      << "] "

#define BUNSAN_LOG_TRACE_INTO(STREAM) BUNSAN_LOG_SEV_INTO(trace, STREAM)
#define BUNSAN_LOG_DEBUG_INTO(STREAM) BUNSAN_LOG_SEV_INTO(debug, STREAM)
#define BUNSAN_LOG_INFO_INTO(STREAM) BUNSAN_LOG_SEV_INTO(info, STREAM)
#define BUNSAN_LOG_WARNING_INTO(STREAM) BUNSAN_LOG_SEV_INTO(warning, STREAM)
#define BUNSAN_LOG_ERROR_INTO(STREAM) BUNSAN_LOG_SEV_INTO(error, STREAM)
#define BUNSAN_LOG_FATAL_INTO(STREAM) BUNSAN_LOG_SEV_INTO(fatal, STREAM)
