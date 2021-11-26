#define BOOST_TEST_MODULE http
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/detail/slist_iterator.hpp>
#include <bunsan/curl/http/error.hpp>
#include <bunsan/curl/http/header.hpp>
#include <bunsan/curl/http/header_parser.hpp>
#include <bunsan/curl/http/header_set.hpp>
#include <bunsan/curl/http/response_head.hpp>
#include <bunsan/curl/http/status.hpp>
#include <bunsan/curl/http_version.hpp>
#include <bunsan/curl/options/wrapper/string_list.hpp>

#include <boost/lexical_cast.hpp>

#include <algorithm>

namespace curl = bunsan::curl;
namespace http = curl::http;

BOOST_AUTO_TEST_SUITE(http_)

BOOST_AUTO_TEST_SUITE(http_version)

BOOST_AUTO_TEST_CASE(pair_to_enum) {
  BOOST_CHECK_EQUAL(curl::http_version_pair(1, 0),
                    curl::http_version::http_1_0);
  BOOST_CHECK_EQUAL(curl::http_version_pair(1, 1),
                    curl::http_version::http_1_1);
  BOOST_CHECK_EQUAL(curl::http_version_pair(2, 0),
                    curl::http_version::http_2_0);
  BOOST_CHECK_THROW(
      static_cast<curl::http_version>(curl::http_version_pair(1, 3)),
      curl::http_version_unsupported_error);
}

BOOST_AUTO_TEST_CASE(enum_to_pair) {
  BOOST_CHECK_EQUAL(curl::http_version_pair(curl::http_version::http_none),
                    curl::http_version_pair());
  BOOST_CHECK_EQUAL(curl::http_version_pair(curl::http_version::http_1_0),
                    curl::http_version_pair(1, 0));
  BOOST_CHECK_EQUAL(curl::http_version_pair(curl::http_version::http_1_1),
                    curl::http_version_pair(1, 1));
  BOOST_CHECK_EQUAL(curl::http_version_pair(curl::http_version::http_2_0),
                    curl::http_version_pair(2, 0));
}

BOOST_AUTO_TEST_CASE(stream) {
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(curl::http_version_pair()),
                    "0.0");
  BOOST_CHECK_EQUAL(
      boost::lexical_cast<std::string>(curl::http_version_pair(1, 2)), "1.2");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                        curl::http_version_pair(curl::http_version::http_1_1)),
                    "1.1");
}

BOOST_AUTO_TEST_SUITE_END()  // http_version

BOOST_AUTO_TEST_SUITE(status)

BOOST_AUTO_TEST_CASE(stream) {
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                        http::status(curl::http_version::http_1_1, 200, "OK")),
                    "HTTP/1.1 200 OK");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                        http::status(curl::http_version::http_1_1, 200, "")),
                    "HTTP/1.1 200");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                        http::status(curl::http_version::http_1_1, 0, "")),
                    "HTTP/1.1");
}

BOOST_AUTO_TEST_CASE(parse) {
  auto r = http::status::parse("HTTP/1.1 200 OK");
  BOOST_CHECK_EQUAL(r.version, curl::http_version::http_1_1);
  BOOST_CHECK_EQUAL(r.code, 200);
  BOOST_CHECK_EQUAL(r.reason, "OK");

  r = http::status::parse("HTTP/1.0 300 hello world");
  BOOST_CHECK_EQUAL(r.version, curl::http_version::http_1_0);
  BOOST_CHECK_EQUAL(r.code, 300);
  BOOST_CHECK_EQUAL(r.reason, "hello world");

  r = http::status::parse("HTTP/2.0 100");
  BOOST_CHECK_EQUAL(r.version, curl::http_version::http_2_0);
  BOOST_CHECK_EQUAL(r.code, 100);
  BOOST_CHECK(r.reason.empty());

  r = http::status::parse("HTTP/1.1");
  BOOST_CHECK_EQUAL(r.version, curl::http_version::http_1_1);
  BOOST_CHECK(!r.code);
  BOOST_CHECK(r.reason.empty());

  r = http::status::parse("HTTP/1.1 ");
  BOOST_CHECK_EQUAL(r.version, curl::http_version::http_1_1);
  BOOST_CHECK(!r.code);
  BOOST_CHECK(r.reason.empty());

  BOOST_CHECK_THROW(http::status::parse("HTTP/1.3 200 OK"),
                    http::status_parse_error);

  BOOST_CHECK_THROW(http::status::parse("HTTP/1"), http::status_parse_error);

  BOOST_CHECK_THROW(http::status::parse("HTTP/-1"), http::status_parse_error);

  BOOST_CHECK_THROW(http::status::parse("HTTP"), http::status_parse_error);

  BOOST_CHECK_THROW(http::status::parse("HTTPS/1.0 200 OK"),
                    http::status_parse_error);
}

BOOST_AUTO_TEST_SUITE_END()  // status

BOOST_AUTO_TEST_SUITE(header)

BOOST_AUTO_TEST_CASE(case_sensitivity) {
  BOOST_CHECK_EQUAL(http::header::parse("Content-Type: text/plain"),
                    http::header("content-type", "text/plain"));
  BOOST_CHECK_EQUAL(http::header::parse("Header: Data"),
                    http::header("header", "Data"));
}

BOOST_AUTO_TEST_CASE(spaces) {
  BOOST_CHECK_EQUAL(http::header::parse("Header: Data   "),
                    http::header("header", "Data"));
  BOOST_CHECK_EQUAL(http::header::parse("Header:Data"),
                    http::header("header", "Data"));
  BOOST_CHECK_EQUAL(http::header::parse("Header:Data   "),
                    http::header("header", "Data"));
  BOOST_CHECK_EQUAL(http::header::parse("Header   :Data"),
                    http::header("header", "Data"));
  BOOST_CHECK_EQUAL(http::header::parse("Header   :Data   "),
                    http::header("header", "Data"));
  BOOST_CHECK_EQUAL(http::header::parse("Header   :  Data   "),
                    http::header("header", "Data"));
}

BOOST_AUTO_TEST_CASE(empty) {
  BOOST_CHECK_EQUAL(http::header::parse("header:"), http::header("header", ""));
  BOOST_CHECK_EQUAL(
      boost::lexical_cast<std::string>(http::header("header", "")), "header:");
}

BOOST_AUTO_TEST_CASE(stream) {
  const http::header u("header", "data1");
  const http::header h("Header", "data1", "data2");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(u), "header: data1");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(h),
                    "header: [\"data1\", \"data2\"]");
}

BOOST_AUTO_TEST_CASE(non_unique_value) {
  const http::header u("header", "data1");
  const http::header h("Header", "data1", "data2");
  BOOST_CHECK_NE(u, h);
  BOOST_CHECK_EQUAL(h, http::header("header", "data1", "data2"));
  BOOST_CHECK_EQUAL(u.value(), "data1");
  BOOST_CHECK_THROW(h.value(), http::header_non_unique_value_error);
}

BOOST_AUTO_TEST_CASE(merge) {
  http::header h("header", "data1");
  BOOST_CHECK_THROW(h.merge(http::header("other", "data2")),
                    http::header_merge_incompatible_name_error);
  h.merge(http::header("Header", "data2"));
  const http::header h3("header", "data3");
  h.merge(h3);
  BOOST_CHECK_EQUAL(h, http::header("header", "data1", "data2", "data3"));
  BOOST_CHECK_EQUAL(
      http::merge_headers(h, http::header("header", "data4"),
                          http::header("header", "data5")),
      http::header("header", "data1", "data2", "data3", "data4", "data5"));
}

BOOST_AUTO_TEST_CASE(plain_headers) {
  http::header h("header", "data1", "", "data2", "data3");
  auto headers =
      boost::copy_range<std::vector<std::string> >(h.plain_headers());
  BOOST_REQUIRE_EQUAL(headers.size(), 4);
  BOOST_CHECK_EQUAL(headers[0], "header: data1");
  BOOST_CHECK_EQUAL(headers[1], "header:");
  BOOST_CHECK_EQUAL(headers[2], "header: data2");
  BOOST_CHECK_EQUAL(headers[3], "header: data3");

  headers.clear();
  for (const std::string &hdr : h.plain_headers()) headers.push_back(hdr);
  BOOST_REQUIRE_EQUAL(headers.size(), 4);
  BOOST_CHECK_EQUAL(headers[0], "header: data1");
  BOOST_CHECK_EQUAL(headers[1], "header:");
  BOOST_CHECK_EQUAL(headers[2], "header: data2");
  BOOST_CHECK_EQUAL(headers[3], "header: data3");

  const curl::options::wrapper::string_list list(h.plain_headers());
  headers.assign(curl::detail::make_slist_string_iterator(list.data()),
                 curl::detail::make_slist_string_iterator());
  BOOST_REQUIRE_EQUAL(headers.size(), 4);
  BOOST_CHECK_EQUAL(headers[0], "header: data1");
  BOOST_CHECK_EQUAL(headers[1], "header:");
  BOOST_CHECK_EQUAL(headers[2], "header: data2");
  BOOST_CHECK_EQUAL(headers[3], "header: data3");
}

BOOST_AUTO_TEST_CASE(plain_headers_iterator) {
  http::header h("header", "data1", "data2", "data3");
  const http::header::plain_const_range range = h.plain_headers();
  http::header::plain_const_iterator i = range.begin();
  BOOST_CHECK_EQUAL(*i++, "header: data1");
  BOOST_CHECK_EQUAL(*i++, "header: data2");
  BOOST_CHECK_EQUAL(*i++, "header: data3");
  BOOST_CHECK(i == range.end());
  i = range.begin();
  BOOST_CHECK_EQUAL(*i++, "header: data1");
  BOOST_CHECK_EQUAL(*i++, "header: data2");
  BOOST_CHECK_EQUAL(*i++, "header: data3");
  BOOST_CHECK(i == range.end());
}

BOOST_AUTO_TEST_SUITE_END()  // header

BOOST_AUTO_TEST_SUITE(header_set)

BOOST_AUTO_TEST_CASE(merge_insert) {
  http::header_set set;
  set.merge_insert(http::header("header1", "data1"));
  set.merge_insert(http::header("header2", "data2"));
  set.merge_insert(http::header("Header1", "data3"));
  set.merge_insert(http::header("Header3", "data4"));
  set.merge_insert(http::header("header3", "data5"));
  BOOST_REQUIRE(set.find("header1") != set.end());
  BOOST_CHECK_EQUAL(*set.find("header1"),
                    http::header("header1", "data1", "data3"));
  BOOST_REQUIRE(set.find("header2") != set.end());
  BOOST_CHECK_EQUAL(*set.find("header2"), http::header("header2", "data2"));
  BOOST_REQUIRE(set.find("header2") != set.end());
  BOOST_CHECK_EQUAL(*set.find("header3"),
                    http::header("header3", "data4", "data5"));
}

BOOST_AUTO_TEST_CASE(plain_headers) {
  http::header_set set;
  set.merge_insert(http::header("header1", "data1"));
  set.merge_insert(http::header("header2", "data2"));
  set.merge_insert(http::header("Header1", "data3"));
  set.merge_insert(http::header("Header3", "data4"));
  set.merge_insert(http::header("header3", "data5"));

  const auto range = set.plain_headers();
  auto hdrs = boost::copy_range<std::vector<std::string> >(range);
  BOOST_REQUIRE_EQUAL(hdrs.size(), 5);
  std::sort(hdrs.begin(), hdrs.end());
  BOOST_CHECK_EQUAL(hdrs[0], "header1: data1");
  BOOST_CHECK_EQUAL(hdrs[1], "header1: data3");
  BOOST_CHECK_EQUAL(hdrs[2], "header2: data2");
  BOOST_CHECK_EQUAL(hdrs[3], "header3: data4");
  BOOST_CHECK_EQUAL(hdrs[4], "header3: data5");

  http::header_set::plain_const_iterator i = range.begin();
  http::header_set::plain_const_iterator j = range.begin();
  BOOST_CHECK(i == j);  // 0, 0
  ++i;
  BOOST_CHECK(i != j);  // 1, 0
  ++j;
  BOOST_CHECK(i == j);  // 1, 1
  ++j;
  BOOST_CHECK(i != j);  // 1, 2
  ++i;
  BOOST_CHECK(i == j);  // 2, 2
  ++i;
  BOOST_CHECK(i != j);  // 3, 2
  ++i;
  BOOST_CHECK(i != j);  // 4, 2
  ++j;
  BOOST_CHECK(i != j);  // 4, 3
  ++j;
  BOOST_CHECK(i == j);  // 4, 4
  ++i;
  BOOST_CHECK(i != j);  // end, 4
  BOOST_CHECK(i == range.end());
  BOOST_CHECK(range.end() == i);
  ++j;
  BOOST_CHECK(i == j);  // end, end
  BOOST_CHECK(j == range.end());
  BOOST_CHECK(range.end() == j);

  i = range.begin();
  hdrs.assign(i, j);  // 0, end
  BOOST_REQUIRE_EQUAL(hdrs.size(), 5);
  std::sort(hdrs.begin(), hdrs.end());
  BOOST_CHECK_EQUAL(hdrs[0], "header1: data1");
  BOOST_CHECK_EQUAL(hdrs[1], "header1: data3");
  BOOST_CHECK_EQUAL(hdrs[2], "header2: data2");
  BOOST_CHECK_EQUAL(hdrs[3], "header3: data4");
  BOOST_CHECK_EQUAL(hdrs[4], "header3: data5");

  hdrs.clear();
  for (const std::string &hdr : range) hdrs.push_back(hdr);
  BOOST_REQUIRE_EQUAL(hdrs.size(), 5);
  std::sort(hdrs.begin(), hdrs.end());
  BOOST_CHECK_EQUAL(hdrs[0], "header1: data1");
  BOOST_CHECK_EQUAL(hdrs[1], "header1: data3");
  BOOST_CHECK_EQUAL(hdrs[2], "header2: data2");
  BOOST_CHECK_EQUAL(hdrs[3], "header3: data4");
  BOOST_CHECK_EQUAL(hdrs[4], "header3: data5");

  const curl::options::wrapper::string_list list(range);
  hdrs.assign(curl::detail::make_slist_string_iterator(list.data()),
              curl::detail::make_slist_string_iterator());
  BOOST_REQUIRE_EQUAL(hdrs.size(), 5);
  std::sort(hdrs.begin(), hdrs.end());
  BOOST_CHECK_EQUAL(hdrs[0], "header1: data1");
  BOOST_CHECK_EQUAL(hdrs[1], "header1: data3");
  BOOST_CHECK_EQUAL(hdrs[2], "header2: data2");
  BOOST_CHECK_EQUAL(hdrs[3], "header3: data4");
  BOOST_CHECK_EQUAL(hdrs[4], "header3: data5");
}

BOOST_AUTO_TEST_CASE(plain_headers_empty) {
  const http::header_set set;
  const auto hdrs =
      boost::copy_range<std::vector<std::string> >(set.plain_headers());
  BOOST_CHECK(hdrs.empty());
}

BOOST_AUTO_TEST_SUITE_END()  // header_set

BOOST_AUTO_TEST_SUITE(response_head)

BOOST_AUTO_TEST_CASE(append) {
  http::response_head head(
      http::status(curl::http_version::http_1_1, 200, "OK"));
  head.append(http::header("header", "data"));
  BOOST_REQUIRE(head.headers().find("header") != head.headers().end());
  BOOST_CHECK_EQUAL(*head.headers().find("header"),
                    http::header("header", "data"));
}

BOOST_AUTO_TEST_SUITE_END()  // response_head

struct header_parser_fixture {
  http::header_parser parser;

  void parse(const std::string &header) { parser.parse(header); }
};

BOOST_FIXTURE_TEST_SUITE(header_parser, header_parser_fixture)

BOOST_AUTO_TEST_CASE(parse_) {
  BOOST_CHECK(parser.empty());

  BOOST_CHECK_THROW(parse("header: data"), http::status_header_expected_error);
  BOOST_CHECK(parser.empty());

  BOOST_CHECK_NO_THROW(parse(""));
  BOOST_CHECK(parser.empty());

  BOOST_CHECK_NO_THROW(parse("HTTP/1.1 200 OK"));
  BOOST_CHECK_EQUAL(parser.size(), 1);
  BOOST_CHECK_EQUAL(parser.last().status(),
                    http::status(curl::http_version::http_1_1, 200, "OK"));
  BOOST_CHECK(parser.last().headers().empty());

  BOOST_CHECK_NO_THROW(parse("HTTP/1.0 404 Not found"));
  BOOST_CHECK_EQUAL(parser.size(), 2);
  BOOST_CHECK_EQUAL(
      parser.last().status(),
      http::status(curl::http_version::http_1_0, 404, "Not found"));
  BOOST_CHECK(parser.last().headers().empty());

  BOOST_CHECK_NO_THROW(parse("header: data1"));
  BOOST_CHECK_EQUAL(parser.size(), 2);
  BOOST_CHECK_EQUAL(parser.last().headers().size(), 1);
  BOOST_REQUIRE(parser.last().headers().find("header") !=
                parser.last().headers().end());
  BOOST_CHECK_EQUAL(parser.last().headers().find("header")->value(), "data1");

  BOOST_CHECK_NO_THROW(parse(""));
  BOOST_CHECK_EQUAL(parser.size(), 2);
  BOOST_CHECK_EQUAL(parser.last().headers().size(), 1);

  BOOST_CHECK_NO_THROW(parse("header: data2"));
  BOOST_CHECK_EQUAL(parser.size(), 2);
  BOOST_CHECK_EQUAL(parser.last().headers().size(), 1);
  BOOST_REQUIRE_EQUAL(parser.last().headers().find("header")->values().size(),
                      2);
  BOOST_CHECK_EQUAL(parser.last().headers().find("header")->values()[0],
                    "data1");
  BOOST_CHECK_EQUAL(parser.last().headers().find("header")->values()[1],
                    "data2");

  BOOST_CHECK_NO_THROW(parse("other: data"));
  BOOST_CHECK_EQUAL(parser.size(), 2);
  BOOST_CHECK_EQUAL(parser.last().headers().size(), 2);
  BOOST_REQUIRE_EQUAL(parser.last().headers().find("other")->values().size(),
                      1);
  BOOST_CHECK_EQUAL(parser.last().headers().find("other")->value(), "data");
}

BOOST_AUTO_TEST_SUITE_END()  // header_parser

BOOST_AUTO_TEST_SUITE_END()  // http_
