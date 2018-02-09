#pragma once

#include <bunsan/utility/utility.hpp>

#include <turtle/mock.hpp>

namespace bunsan::utility {

MOCK_BASE_CLASS(mock_utility, utility) {
  MOCK_METHOD(setup, 1, void(const boost::property_tree::ptree &ptree))
};

}  // namespace bunsan::utility
