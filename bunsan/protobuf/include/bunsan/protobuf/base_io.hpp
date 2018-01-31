#pragma once

#include <boost/noncopyable.hpp>

namespace bunsan {
namespace protobuf {

class base_io : private boost::noncopyable {
 public:
  base_io();
  virtual ~base_io();

  void allow_partial(const bool allow_partial_) {
    m_allow_partial = allow_partial_;
  }

  bool allow_partial() const { return m_allow_partial; }

 private:
  bool m_allow_partial = false;
};

}  // namespace protobuf
}  // namespace bunsan
