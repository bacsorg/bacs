#pragma once

#include <boost/parameter/keyword.hpp>

namespace bunsan {
namespace log {
namespace keywords {

BOOST_PARAMETER_KEYWORD(tag, file)
BOOST_PARAMETER_KEYWORD(tag, line)
BOOST_PARAMETER_KEYWORD(tag, function)
BOOST_PARAMETER_KEYWORD(tag, pretty_function)

}  // namespace keywords
}  // namespace log
}  // namespace bunsan
