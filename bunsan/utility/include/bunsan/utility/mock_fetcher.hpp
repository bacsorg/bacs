#pragma once

#include <bunsan/utility/fetcher.hpp>

#include <turtle/mock.hpp>

namespace bunsan::utility {

MOCK_BASE_CLASS(mock_fetcher, fetcher) {
  MOCK_METHOD(fetch, 2, void(const std::string &uri,
                             const boost::filesystem::path &dst))
};

}  // namespace bunsan::utility
