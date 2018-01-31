#pragma once

#include <bunsan/test/filesystem/tempfiles.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

struct tempfile : private tempfiles {
  tempfile();

  boost::filesystem::path path;
};

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
