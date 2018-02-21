#pragma once

#include <bunsan/test/getenv.hpp>

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <functional>
#include <string>

struct server_fixture {
  const std::string url_root;
  std::string wdata;
  std::string rdata;

  server_fixture() : url_root(get_url_root()) {}

  std::function<std::size_t(const char *, std::size_t)> writer =
      [this](const char *const ptr, const std::size_t size) {
        wdata.append(ptr, size);
        return size;
      };

  std::function<std::size_t(char *, std::size_t)> reader =
      [this](char *const ptr, std::size_t size) {
        size = std::min(size, rdata.size());
        std::copy(rdata.begin(), rdata.begin() + size, ptr);
        rdata.erase(rdata.begin(), rdata.begin() + size);
        return size;
      };

 private:
  static std::string get_url_root() {
    return str(boost::format("http://localhost:%d") % get_port());
  }

  static int get_port() {
    const int port =
        boost::lexical_cast<int>(bunsan::test::getenv("BUNSAN_PORT"));
    BOOST_ASSERT(port > 0);
    return port;
  }
};
