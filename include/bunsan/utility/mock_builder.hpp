#pragma once

#include <bunsan/utility/builder.hpp>

#include <turtle/mock.hpp>

namespace bunsan {
namespace utility {

MOCK_BASE_CLASS(mock_builder, builder) {
  MOCK_METHOD(install, 3, void(const boost::filesystem::path &src,
                               const boost::filesystem::path &bin,
                               const boost::filesystem::path &root))
  MOCK_METHOD(pack, 4, void(const boost::filesystem::path &src,
                            const boost::filesystem::path &bin,
                            const boost::filesystem::path &archive,
                            const archiver_ptr &archiver_))
};

}  // namespace utility
}  // namespace bunsan
