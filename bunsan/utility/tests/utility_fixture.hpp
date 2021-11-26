#pragma once

#include <bunsan/process/mock_executor.hpp>
#include <bunsan/utility/mock_resolver.hpp>

namespace bu = bunsan::utility;

struct utility_fixture : bunsan::process::mock_executor_fixture {
  bu::mock_resolver resolver;
};
