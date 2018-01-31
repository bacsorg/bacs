#pragma once

#include <bunsan/curl/info/wrapper/getinfo.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>
#include <bunsan/curl/info/wrapper/string.hpp>

namespace bunsan {
namespace curl {
namespace info {

BUNSAN_CURL_GETINFO(rtsp_session_id, CURLINFO_RTSP_SESSION_ID, string)
BUNSAN_CURL_GETINFO(rtsp_client_cseq, CURLINFO_RTSP_CLIENT_CSEQ, long)
BUNSAN_CURL_GETINFO(rtsp_server_cseq, CURLINFO_RTSP_SERVER_CSEQ, long)
BUNSAN_CURL_GETINFO(rtsp_cseq_recv, CURLINFO_RTSP_CSEQ_RECV, long)

}  // namespace info
}  // namespace curl
}  // namespace bunsan
