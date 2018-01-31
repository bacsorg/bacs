#pragma once

#include <bunsan/asio/async_result.hpp>
#include <bunsan/error.hpp>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <functional>
#include <iomanip>
#include <sstream>

namespace bunsan {
namespace asio {

struct invalid_data_error : virtual error {
  using data = boost::error_info<struct tag_data, std::string>;
};

template <typename Connection>
class line_connection : private boost::noncopyable {
 public:
  static constexpr char delimiter = '\n';

 public:
  explicit line_connection(Connection &connection) : m_connection(connection) {}

  template <typename Handler>
  BUNSAN_ASIO_INITFN_RESULT_TYPE(Handler, void(boost::system::error_code))
      async_write(const std::string &data, Handler &&handler) {
    BUNSAN_ASIO_INITFN_BEGIN(Handler, handler, void(boost::system::error_code),
                             real_handler, result)

    if (data.find(delimiter) != std::string::npos)
      BOOST_THROW_EXCEPTION(invalid_data_error()
                            << invalid_data_error::data(data)
                            << invalid_data_error::message("\\n is forbidden"));

    std::vector<boost::asio::const_buffer> buffers = {
        boost::asio::buffer(data), boost::asio::buffer(eoln)};
    boost::asio::async_write(
        m_connection, buffers,
        [real_handler](const boost::system::error_code &ec,
                       const std::size_t) mutable { real_handler(ec); });

    BUNSAN_ASIO_INITFN_END(result)
  }

  template <typename Handler>
  BUNSAN_ASIO_INITFN_RESULT_TYPE(Handler, void(boost::system::error_code))
      async_read(std::string &data, Handler &&handler) {
    BUNSAN_ASIO_INITFN_BEGIN(Handler, handler, void(boost::system::error_code),
                             real_handler, result)

    boost::asio::async_read_until(
        m_connection, m_inbound_data, delimiter,
        [this, &data, real_handler](const boost::system::error_code &ec,
                                    const std::size_t n) {
          handle_read(ec, n, data, real_handler);
        });

    BUNSAN_ASIO_INITFN_END(result)
  }

  boost::asio::io_service &get_io_service() {
    return m_connection.get_io_service();
  }

  void close() { m_connection.close(); }

 private:
  template <typename Handler>
  void handle_read(const boost::system::error_code &ec, const std::size_t n,
                   std::string &data, Handler handler) {
    if (n >= 1) {
      BOOST_ASSERT(n <= m_inbound_data.size());
      const auto bufs = m_inbound_data.data();
      data.assign(buffers_begin(bufs), buffers_begin(bufs) + n - 1);
      BOOST_ASSERT(*(buffers_begin(bufs) + n - 1) == delimiter);
      m_inbound_data.consume(n);
    }
    handler(ec);
  }

 private:
  Connection &m_connection;

  boost::asio::streambuf m_inbound_data;

  static const char eoln[1];
};

template <typename Connection>
const char line_connection<Connection>::eoln[1] = {
    line_connection<Connection>::delimiter};

}  // namespace asio
}  // namespace bunsan
