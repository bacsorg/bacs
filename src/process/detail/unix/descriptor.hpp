#pragma once

#include <boost/optional.hpp>

namespace bunsan {
namespace process {
namespace detail {

class descriptor {
 public:
  descriptor() = default;
  ~descriptor();

  explicit descriptor(int fd);
  descriptor(int fd, bool close);

  descriptor(const descriptor &) = delete;
  descriptor(descriptor &&);

  descriptor &operator=(const descriptor &) = delete;
  descriptor &operator=(descriptor &&);

  explicit operator bool() const { return static_cast<bool>(m_fd); }
  int operator*() const { return *m_fd; }

  void reset();
  void reset(const int fd);
  void close();
  void close_no_except() noexcept;

  void swap(descriptor &o) noexcept {
    using std::swap;

    swap(m_fd, o.m_fd);
    swap(m_close, o.m_close);
  }

  descriptor dup() const;
  descriptor dup2(int new_fd) const;

 private:
  boost::optional<int> m_fd;
  bool m_close = true;
};

inline void swap(descriptor &a, descriptor &b) noexcept { a.swap(b); }

descriptor stdin_descriptor();
descriptor stdout_descriptor();
descriptor stderr_descriptor();

}  // namespace detail
}  // namespace process
}  // namespace bunsan
