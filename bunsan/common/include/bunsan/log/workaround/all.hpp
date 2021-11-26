#pragma once

#include <boost/version.hpp>

#if BOOST_VERSION / 100 == 1059
#include <bunsan/log/workaround/boost_1_59.hpp>
#endif
