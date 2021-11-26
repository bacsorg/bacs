#pragma once

#include <bunsan/utility/archiver.hpp>

#include <turtle/mock.hpp>

namespace bunsan::utility {

MOCK_BASE_CLASS(mock_archiver, archiver) {
  MOCK_METHOD(pack, 2, void(const boost::filesystem::path &archive,
                            const boost::filesystem::path &file))
  MOCK_METHOD(pack_contents, 2,
              void(const boost::filesystem::path &archive,
                   const boost::filesystem::path &dir))
  MOCK_METHOD(unpack, 2, void(const boost::filesystem::path &archive,
                              const boost::filesystem::path &dir))
};

}  // namespace bunsan::utility
