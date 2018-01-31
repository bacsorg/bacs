#define BOOST_TEST_MODULE http_client
#include <boost/test/unit_test.hpp>

#include "server_fixture.hpp"

#include <bunsan/curl/http/client.hpp>
#include <bunsan/curl/http/error.hpp>

namespace curl = bunsan::curl;
namespace http = curl::http;

struct client_fixture : server_fixture {
  http::client client;
};

BOOST_FIXTURE_TEST_SUITE(http_client, client_fixture)

BOOST_AUTO_TEST_CASE(perform) {
  client.set_url(url_root + "/hello");
  BOOST_CHECK_EQUAL(client.perform().code, 200);
}

BOOST_AUTO_TEST_CASE(set_header) {
  client.set_url(url_root + "/response");
  client.set_header("X-cURL-Response-Code", "404");
  client.set_header("X-cURL-Response-Reason", "Not found");
  const http::status status = client.perform();
  // version is not important
  BOOST_CHECK_EQUAL(status.code, 404);
  BOOST_CHECK_EQUAL(status.reason, "Not found");
}

BOOST_AUTO_TEST_CASE(check_perform) {
  client.set_url(url_root + "/hello");
  BOOST_CHECK_NO_THROW(client.check_perform());

  client.set_url(url_root + "/response");
  client.set_header("X-cURL-Response-Code", "404");
  client.set_header("X-cURL-Response-Reason", "Not found");
  BOOST_CHECK_THROW(client.check_perform(), http::not_ok_status_error);
}

BOOST_AUTO_TEST_CASE(post_data) {
  client.set_url(url_root + "/echo");
  client.set_writefunction(writer);
  client.set_post_data("Hello, world!");
  BOOST_CHECK_NO_THROW(client.check_perform());
  BOOST_CHECK_EQUAL(wdata, "Hello, world!");
}

BOOST_AUTO_TEST_CASE(readfunction_with_size) {
  rdata = "Hello, world!";
  client.set_url(url_root + "/echo");
  client.set_content_length(rdata.size());
  client.set_readfunction(reader);
  client.set_writefunction(writer);
  BOOST_CHECK_NO_THROW(client.check_perform());
  BOOST_CHECK_EQUAL(wdata, "Hello, world!");
}

BOOST_AUTO_TEST_CASE(readfunction_chunked) {
  rdata = "Hello, world!";
  client.set_url(url_root + "/echo");
  client.set_transfer_encoding_chunked();
  client.set_readfunction(reader);
  client.set_writefunction(writer);
  BOOST_CHECK_NO_THROW(client.check_perform());
  BOOST_CHECK_EQUAL(wdata, "Hello, world!");
}

BOOST_AUTO_TEST_SUITE_END()  // http_client
