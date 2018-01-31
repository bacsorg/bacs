#pragma once

#include <curl/curl.h>

namespace bunsan {
namespace curl {

enum class redir_post : long {
  http301 = CURL_REDIR_POST_301,
  http302 = CURL_REDIR_POST_302,
  http303 = CURL_REDIR_POST_303,
  all = CURL_REDIR_POST_ALL
};

}  // namespace curl
}  // namespace bunsan
