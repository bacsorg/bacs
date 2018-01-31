#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <deque>
#include <string>

namespace bunsan {
namespace asio {

template <typename Source, typename Sink>
class buffer_connection {
 public:
  using handler = boost::function<void(boost::system::error_code, std::size_t)>;

  // note: (nullptr, 0) on EOF or close
  using data_handler = boost::function<void(const char *, std::size_t)>;

 public:
  buffer_connection(Source &source, Sink &sink, const handler &handle_read,
                    const handler &handle_write)
      : m_strand(source.get_io_service()),
        m_handle_read(handle_read),
        m_handle_write(handle_write),
        m_source(source),
        m_sink(sink) {}

  buffer_connection(Source &source, Sink &sink)
      : buffer_connection(source, sink, handler{}, handler{}) {}

  void set_read_data_handler(const data_handler &handle_data) {
    m_handle_read_data = handle_data;
  }

  void set_write_data_handler(const data_handler &handle_data) {
    m_handle_write_data = handle_data;
  }

  bool close_sink_on_eof() const { return m_close_sink_on_eof; }

  /// \warning not thread safe, must be executed before start()
  void set_close_sink_on_eof(const bool value = true) {
    m_close_sink_on_eof = value;
  }

  bool discard_on_sink_error() const { return m_discard_on_sink_error; }

  /// \warning not thread safe, must be executed before start()
  void set_discard_on_sink_error(const bool value = true) {
    m_discard_on_sink_error = value;
  }

  bool empty() const { return m_queue.empty(); }

  void start() {
    m_strand.dispatch(boost::bind(&buffer_connection::spawn_reader, this));
  }

  boost::asio::io_service &get_io_service() {
    return m_source.get_io_service();
  }

  void close() {
    m_strand.dispatch(boost::bind(&buffer_connection::close_, this));
  }

  void terminate() {
    m_strand.post(boost::bind(&buffer_connection::terminate_, this));
  }

 private:
  void close_() {
    if (m_closed) return;

    m_closed = true;
    if (m_queue.empty()) m_sink.close();
  }

  void terminate_() {
    m_closed = true;
    m_terminated = true;
    m_source.close();
    m_sink.close();
  }

  void spawn_reader() {
    if (m_terminated) return;

    m_source.async_read_some(
        boost::asio::buffer(m_inbound_data),
        m_strand.wrap(
            boost::bind(&buffer_connection::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
  }

  void spawn_writer() {
    if (m_terminated) return;

    if (!m_sink_ok) {
      if (m_discard_on_sink_error) m_queue.clear();
      return;
    }

    const std::vector<char> &buffer = m_queue.front();
    boost::asio::async_write(
        m_sink, boost::asio::buffer(buffer),
        m_strand.wrap(
            boost::bind(&buffer_connection::handle_write, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
  }

  void handle_read(const boost::system::error_code &ec,
                   const std::size_t size) {
    if (m_handle_read) m_handle_read(ec, size);

    if (size && (m_sink_ok || !m_discard_on_sink_error)) {
      m_queue.emplace_back(m_inbound_data, m_inbound_data + size);
      if (m_handle_read_data) m_handle_read_data(m_inbound_data, size);

      if (m_queue.size() == 1) {  // queue was empty before, writer is required
        spawn_writer();
      }
    }

    if (ec) {
      if (m_handle_read_data) m_handle_read_data(nullptr, 0);
      m_source_ok = false;
    } else if (!m_closed) {
      spawn_reader();
    }

    if (m_queue.empty()) try_close_sink();
  }

  void handle_write(const boost::system::error_code &ec,
                    const std::size_t size) {
    if (m_handle_write) m_handle_write(ec, size);

    if (size) {
      if (m_handle_write_data)
        m_handle_write_data(m_queue.front().data(), size);
      if (size < m_queue.front().size()) {
        BOOST_ASSERT(ec);
        m_queue.front().erase(m_queue.front().begin(),
                              m_queue.front().begin() + size);
      } else {
        BOOST_ASSERT(size == m_queue.front().size());
        m_queue.pop_front();
      }
    }

    if (ec) {
      m_sink_ok = false;
      return;
    }

    if (m_queue.empty()) {
      try_close_sink();
    } else {
      spawn_writer();
    }
  }

  void try_close_sink() {
    if (m_closed || (!m_source_ok && m_close_sink_on_eof)) {
      if (m_handle_write_data) m_handle_write_data(nullptr, 0);
      m_sink.close();
    }
  }

 private:
  boost::asio::io_service::strand m_strand;
  handler m_handle_read;
  handler m_handle_write;
  data_handler m_handle_read_data;
  data_handler m_handle_write_data;
  Source &m_source;
  Sink &m_sink;

  char m_inbound_data[4096];
  std::deque<std::vector<char>> m_queue;
  bool m_closed = false;
  bool m_terminated = false;

  bool m_source_ok = true;
  bool m_sink_ok = true;

  bool m_close_sink_on_eof = true;
  bool m_discard_on_sink_error = false;
};

}  // namespace asio
}  // namespace bunsan
