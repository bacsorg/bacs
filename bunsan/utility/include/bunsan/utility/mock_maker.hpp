#pragma once

#include <bunsan/utility/maker.hpp>

#include <turtle/mock.hpp>

namespace bunsan::utility {

MOCK_BASE_CLASS(mock_maker, maker){MOCK_METHOD(
    exec, 3,
    void(const boost::filesystem::path &cwd,
         const std::vector<std::string> &targets,
         const std::unordered_map<std::string, std::string> &flags))};

}  // namespace bunsan::utility
