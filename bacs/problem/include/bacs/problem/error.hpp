#pragma once

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

#include <boost/filesystem/path.hpp>

namespace bacs::problem {

struct error : virtual bunsan::error {
  using path = bunsan::filesystem::error::path;
};

}  // namespace bacs::problem
