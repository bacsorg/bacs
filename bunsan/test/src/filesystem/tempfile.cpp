#include <bunsan/test/filesystem/tempfile.hpp>

namespace bunsan::test::filesystem {

tempfile::tempfile() : path(allocate()) {}

}  // namespace bunsan::test::filesystem
