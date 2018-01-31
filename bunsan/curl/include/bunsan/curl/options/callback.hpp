#pragma once

#include <bunsan/curl/options/wrapper/c_function.hpp>
#include <bunsan/curl/options/wrapper/closesocketfunction.hpp>
#include <bunsan/curl/options/wrapper/debugfunction.hpp>
#include <bunsan/curl/options/wrapper/fnmatch_function.hpp>
#include <bunsan/curl/options/wrapper/headerfunction.hpp>
#include <bunsan/curl/options/wrapper/interleavefunction.hpp>
#include <bunsan/curl/options/wrapper/ioctlfunction.hpp>
#include <bunsan/curl/options/wrapper/opensocketfunction.hpp>
#include <bunsan/curl/options/wrapper/progressfunction.hpp>
#include <bunsan/curl/options/wrapper/readfunction.hpp>
#include <bunsan/curl/options/wrapper/seekfunction.hpp>
#include <bunsan/curl/options/wrapper/sockoptfunction.hpp>
#include <bunsan/curl/options/wrapper/ssl_ctx_function.hpp>
#include <bunsan/curl/options/wrapper/wrapped_callback.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>
#include <bunsan/curl/options/wrapper/writefunction.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(writefunction, CURLOPT_WRITEFUNCTION,
                                    CURLOPT_WRITEDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(readfunction, CURLOPT_READFUNCTION,
                                    CURLOPT_READDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(ioctlfunction, CURLOPT_IOCTLFUNCTION,
                                    CURLOPT_IOCTLDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(seekfunction, CURLOPT_SEEKFUNCTION,
                                    CURLOPT_SEEKDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(sockoptfunction, CURLOPT_SOCKOPTFUNCTION,
                                    CURLOPT_SOCKOPTDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(opensocketfunction,
                                    CURLOPT_OPENSOCKETFUNCTION,
                                    CURLOPT_OPENSOCKETDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(closesocketfunction,
                                    CURLOPT_CLOSESOCKETFUNCTION,
                                    CURLOPT_CLOSESOCKETDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(progressfunction, CURLOPT_PROGRESSFUNCTION,
                                    CURLOPT_PROGRESSDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(xferinfofunction, CURLOPT_XFERINFOFUNCTION,
                                    CURLOPT_XFERINFODATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(headerfunction, CURLOPT_HEADERFUNCTION,
                                    CURLOPT_HEADERDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(debugfunction, CURLOPT_DEBUGFUNCTION,
                                    CURLOPT_DEBUGDATA)
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(ssl_ctx_function, CURLOPT_SSL_CTX_FUNCTION,
                                    CURLOPT_SSL_CTX_DATA)

BUNSAN_CURL_OPTION_WRAPPED(conv_to_network_function,
                           CURLOPT_CONV_TO_NETWORK_FUNCTION,
                           c_function<curl_conv_callback>)
BUNSAN_CURL_OPTION_WRAPPED(conv_from_network_function,
                           CURLOPT_CONV_FROM_NETWORK_FUNCTION,
                           c_function<curl_conv_callback>)
BUNSAN_CURL_OPTION_WRAPPED(conv_from_utf8_function,
                           CURLOPT_CONV_FROM_UTF8_FUNCTION,
                           c_function<curl_conv_callback>)

BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(interleavefunction,
                                    CURLOPT_INTERLEAVEFUNCTION,
                                    CURLOPT_INTERLEAVEDATA)
#if 0
BUNSAN_CURL_CALLBACK_PAIR_WRAPPED(
    chunk_function_pair,
    CURLOPT_CHUNK_BGN_FUNCTION,
    CURLOPT_CHUNK_END_FUNCTION,
    CURLOPT_CHUNK_DATA,
    long (const void *transfer_info, void *ptr, int remains),
    long (void *ptr)
)
#endif
BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(fnmatch_function, CURLOPT_FNMATCH_FUNCTION,
                                    CURLOPT_FNMATCH_DATA)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
