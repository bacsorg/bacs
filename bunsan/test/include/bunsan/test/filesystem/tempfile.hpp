#pragma once

#include <bunsan/test/filesystem/tempfiles.hpp>

namespace bunsan::test::filesystem {

struct tempfile : private tempfiles {
  tempfile();

  boost::filesystem::path path;
};

}  // namespace bunsan::test::filesystem
