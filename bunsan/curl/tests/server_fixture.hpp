#pragma once

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <functional>
#include <string>

struct server_fixture {
  const std::string url_root = "http://localhost:8090";
  std::string wdata;
  std::string rdata;

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
};
