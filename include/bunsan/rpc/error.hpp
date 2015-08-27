#pragma once

#include <bunsan/error.hpp>
#include <bunsan/rpc/status.hpp>

namespace bunsan {
namespace rpc {

struct error : virtual bunsan::error {
  using rpc_method = boost::error_info<struct tag_rpc_method, std::string>;
  using rpc_status = boost::error_info<struct tag_rpc_status, grpc::Status>;
};

}  // namespace rpc
}  // namespace bunsan

/*namespace boost {
std::string to_string(const bunsan::rpc::error::rpc_status &status);
}  // namespace boost
*/
