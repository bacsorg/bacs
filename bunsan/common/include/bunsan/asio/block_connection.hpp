#pragma once

#include <bunsan/asio/async_result.hpp>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <functional>
#include <iomanip>
#include <sstream>

namespace bunsan {
namespace asio {

template <typename Connection>
class block_connection : private boost::noncopyable {
 public:
  explicit block_connection(Connection &connection)
      : m_connection(connection) {}

  template <typename Handler>
  BUNSAN_ASIO_INITFN_RESULT_TYPE(Handler, void(boost::system::error_code))
      async_write(const std::string &data, Handler &&handler) {
    BUNSAN_ASIO_INITFN_BEGIN(Handler, handler, void(boost::system::error_code),
                             real_handler, result)

    std::ostringstream header_stream;
    const header size = data.size();
    header_stream << std::setw(HEADER_SIZE) << std::hex << size;
    if (!header_stream || header_stream.str().size() != HEADER_SIZE) {
      boost::system::error_code error(boost::asio::error::invalid_argument);
      get_io_service().post(std::bind(real_handler, error));
    }
    m_outbound_header = header_stream.str();

    std::vector<boost::asio::const_buffer> buffers = {
        boost::asio::buffer(m_outbound_header), boost::asio::buffer(data)};
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

    boost::asio::async_read(
        m_connection, boost::asio::buffer(m_inbound_header),
        [this, &data, real_handler](const boost::system::error_code &ec,
                                    const std::size_t) {
          handle_read_header(ec, data, real_handler);
        });

    BUNSAN_ASIO_INITFN_END(result)
  }

  boost::asio::io_service &get_io_service() {
    return m_connection.get_io_service();
  }

  void close() { m_connection.close(); }

 private:
  template <typename Handler>
  void handle_read_header(const boost::system::error_code &ec,
                          std::string &data, Handler handler) {
    if (ec) {
      handler(ec);
    } else {
      std::istringstream is(std::string(m_inbound_header, HEADER_SIZE));
      header size;
      if (!(is >> std::hex >> size)) {
        boost::system::error_code error(boost::asio::error::invalid_argument);
        handler(error);
        return;
      }

      m_inbound_data.resize(size);

      boost::asio::async_read(
          m_connection, boost::asio::buffer(m_inbound_data),
          [this, &data, handler](const boost::system::error_code &ec,
                                 const std::size_t) {
            handle_read_data(ec, data, handler);
          });
    }
  }

  template <typename Handler>
  void handle_read_data(const boost::system::error_code &ec, std::string &data,
                        Handler handler) {
    if (ec) {
      handler(ec);
    } else {
      data.assign(m_inbound_data.data(), m_inbound_data.size());
      handler(ec);
    }
  }

 private:
  Connection &m_connection;

  using header = std::uint64_t;

  /// Size of hex representation
  static constexpr std::size_t HEADER_SIZE = sizeof(header) * 2;

  char m_inbound_header[HEADER_SIZE];
  std::vector<char> m_inbound_data;

  std::string m_outbound_header;
};

}  // namespace asio
}  // namespace bunsan
