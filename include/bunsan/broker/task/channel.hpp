#pragma once

#include <bunsan/broker/protocol.pb.h>

#include <boost/noncopyable.hpp>

namespace bunsan {
namespace broker {
namespace task {

class channel : private boost::noncopyable {
 public:
  virtual ~channel() {}

  virtual void send_status(const Status &status) = 0;
  virtual void send_result(const Result &result) = 0;
};

}  // namespace task
}  // namespace broker
}  // namespace bunsan
