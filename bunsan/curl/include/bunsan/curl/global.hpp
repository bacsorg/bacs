#pragma once

#include <curl/curl.h>

#include <boost/noncopyable.hpp>

namespace bunsan {
namespace curl {

/// \warning This class is not thread-safe.
class global : private boost::noncopyable {
 public:
  explicit global(const long flags = CURL_GLOBAL_DEFAULT);

  global(const long flags, const curl_malloc_callback m,
         const curl_free_callback f, const curl_realloc_callback r,
         const curl_strdup_callback s, const curl_calloc_callback c);

  ~global();

 private:
  /*!
   * Library should be initialized by this object.
   *
   * \warning Currently there is no way to initialize library
   * with non-default constructor.
   *
   * \todo Non-default library initialization.
   */
  static const global static_instance;
};

}  // namespace curl
}  // namespace bunsan
