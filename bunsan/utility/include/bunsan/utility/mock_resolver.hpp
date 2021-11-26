#pragma once

#include <bunsan/utility/resolver.hpp>

#include <turtle/mock.hpp>

namespace bunsan::utility {

MOCK_BASE_CLASS(mock_resolver, resolver) {
  MOCK_METHOD(find_executable, 1,
              boost::filesystem::path(const boost::filesystem::path &exe))
  MOCK_METHOD(find_library, 1,
              boost::filesystem::path(const boost::filesystem::path &lib))
};

}  // namespace bunsan::utility
