#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <deque>
#include <utility>

namespace bunsan {
namespace asio {

template <typename T, typename Connection>
class queued_writer : private boost::noncopyable {
 public:
  using write_handler = boost::function<void(boost::system::error_code)>;

 public:
  queued_writer(Connection &connection,
                const write_handler &handle_write_custom)
      : m_connection(connection),
        m_strand(m_connection.get_io_service()),
        m_handle_write_custom(handle_write_custom) {}

  explicit queued_writer(Connection &connection)
      : queued_writer(connection, write_handler()) {}

  void write(T object) {
    m_strand.post(boost::bind(&queued_writer<T, Connection>::push, this,
                              std::move(object)));
  }

  void close() {
    m_strand.post(boost::bind(&queued_writer<T, Connection>::close_, this));
  }

  void terminate() {
    m_strand.post(boost::bind(&queued_writer<T, Connection>::terminate_, this));
  }

 private:
  void push(T object) {
    m_queue.push_back(std::move(object));
    BOOST_ASSERT(!m_queue.empty());
    if (m_queue.size() == 1) {  // queue was empty before, writer is required
      spawn_writer();
    }
  }

  void close_() {
    if (m_last) return;

    m_last = true;
    if (m_queue.empty()) m_connection.close();
  }

  void terminate_() {
    m_last = true;
    m_connection.close();
  }

  void spawn_writer() {
    // note: std::deque never invalidates
    // external references on push/pop.
    const T &object = m_queue.front();
    m_connection.async_write(
        object,
        m_strand.wrap(boost::bind(&queued_writer<T, Connection>::handle_write,
                                  this, boost::asio::placeholders::error)));
  }

  void handle_write(const boost::system::error_code &ec) {
    if (m_handle_write_custom) m_handle_write_custom(ec);

    if (ec) return;

    m_queue.pop_front();
    if (m_queue.empty()) {
      if (m_last) m_connection.close();
    } else {
      spawn_writer();
    }
  }

 private:
  Connection &m_connection;
  boost::asio::io_service::strand m_strand;
  std::deque<T> m_queue;
  bool m_last = false;
  write_handler m_handle_write_custom;
};

}  // namespace asio
}  // namespace bunsan
