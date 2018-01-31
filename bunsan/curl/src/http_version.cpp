#include <bunsan/curl/http_version.hpp>

#include <initializer_list>

namespace bunsan {
namespace curl {

http_version_pair::http_version_pair(const http_version version) {
  switch (version) {
    case http_version::http_none:
      // default initialization
      break;
    case http_version::http_1_0:
      *this = {1, 0};
      break;
    case http_version::http_1_1:
      *this = {1, 1};
      break;
    case http_version::http_2_0:
    case http_version::http_2tls:
    case http_version::http_2_prior_knowledge:
      *this = {2, 0};
      break;
  }
}

http_version_pair::operator http_version() const {
  switch (version_major) {
    case 1:
      switch (version_minor) {
        case 0:
          return http_version::http_1_0;
        case 1:
          return http_version::http_1_1;
      }
      break;
    case 2:
      switch (version_minor) {
        case 0:
          return http_version::http_2_0;
      }
      break;
  }
  BOOST_THROW_EXCEPTION(
      http_version_unsupported_error()
      << http_version_unsupported_error::version_major(version_major)
      << http_version_unsupported_error::version_minor(version_minor));
}

std::ostream &operator<<(std::ostream &out, const http_version_pair &version) {
  return out << version.version_major << "." << version.version_minor;
}

}  // namespace curl
}  // namespace bunsan
