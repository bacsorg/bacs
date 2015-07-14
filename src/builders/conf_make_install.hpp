#pragma once

#include <bunsan/utility/builder.hpp>

namespace bunsan {
namespace utility {
namespace builders {

struct conf_make_install_error : virtual builder_install_error {};
struct conf_make_install_configure_error : virtual conf_make_install_error {};
struct conf_make_install_make_error : virtual conf_make_install_error {};
struct conf_make_install_install_error : virtual conf_make_install_error {};

class conf_make_install : public builder {
 public:
  void install(const boost::filesystem::path &src,
               const boost::filesystem::path &bin,
               const boost::filesystem::path &root) override;

 protected:
  virtual void configure_(const boost::filesystem::path &src,
                          const boost::filesystem::path &bin) = 0;

  virtual void make_(const boost::filesystem::path &src,
                     const boost::filesystem::path &bin) = 0;

  virtual void install_(const boost::filesystem::path &src,
                        const boost::filesystem::path &bin,
                        const boost::filesystem::path &root) = 0;
};

}  // namespace builders
}  // namespace utility
}  // namespace bunsan
