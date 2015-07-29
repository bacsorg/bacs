#pragma once

#include <bunsan/broker/task/channel.hpp>

#include <turtle/mock.hpp>

namespace bunsan {
namespace broker {
namespace task {

MOCK_BASE_CLASS(mock_channel, channel) {
  MOCK_METHOD(send_status, 1, void(const Status &status))
  MOCK_METHOD(send_result, 1, void(const Result &result))
};

}  // namespace task
}  // namespace broker
}  // namespace bunsan
