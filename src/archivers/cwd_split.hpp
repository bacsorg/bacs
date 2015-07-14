#pragma once

#include <bunsan/utility/archiver.hpp>

namespace bunsan {
namespace utility {
namespace archivers {

struct cwd_split_error : virtual archiver_error {};
struct cwd_split_pack_from_error : virtual cwd_split_error {
  typedef boost::error_info<struct tag_cwd, boost::filesystem::path> cwd;
};

class cwd_split : public archiver {
 public:
  void pack(const boost::filesystem::path &archive,
            const boost::filesystem::path &file) override;

  void pack_contents(const boost::filesystem::path &archive,
                     const boost::filesystem::path &dir) override;

 protected:
  virtual void pack_from(const boost::filesystem::path &cwd,
                         const boost::filesystem::path &archive,
                         const boost::filesystem::path &file) = 0;
};

}  // namespace archivers
}  // namespace utility
}  // namespace bunsan
