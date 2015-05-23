#pragma once

#include <bunsan/process/mock_executor.hpp>
#include <bunsan/utility/resolver.hpp>

namespace bu = bunsan::utility;

struct utility_fixture: bunsan::process::mock_executor_fixture
{
    bu::resolver resolver;
};
