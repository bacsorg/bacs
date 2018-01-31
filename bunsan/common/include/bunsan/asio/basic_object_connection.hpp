#pragma once

#include <bunsan/asio/async_result.hpp>
#include <bunsan/asio/block_connection.hpp>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <sstream>

namespace bunsan {
namespace asio {

template <typename IArchive, typename OArchive, typename Connection>
class basic_object_connection : private boost::noncopyable {
 public:
  explicit basic_object_connection(Connection &connection)
      : m_block_connection(connection) {}

  template <typename T, typename Handler>
  BUNSAN_ASIO_INITFN_RESULT_TYPE(Handler, void(boost::system::error_code))
      async_write(const T &obj, Handler &&handler) {
    std::ostringstream sout;
    {
      OArchive oa(sout);
      oa << obj;
    }
    m_outbound_data = sout.str();
    return m_block_connection.async_write(m_outbound_data,
                                          std::forward<Handler>(handler));
  }

  template <typename T, typename Handler>
  BUNSAN_ASIO_INITFN_RESULT_TYPE(Handler, void(boost::system::error_code))
      async_read(T &obj, Handler &&handler) {
    BUNSAN_ASIO_INITFN_BEGIN(Handler, handler, void(boost::system::error_code),
                             real_handler, result)

    m_block_connection.async_read(
        m_inbound_data,
        [this, &obj, real_handler](const boost::system::error_code &ec) {
          handle_read(ec, obj, real_handler);
        });

    BUNSAN_ASIO_INITFN_END(result)
  }

  boost::asio::io_service &get_io_service() {
    return m_block_connection.get_io_service();
  }

  void close() { m_block_connection.close(); }

 private:
  template <typename T, typename Handler>
  void handle_read(const boost::system::error_code &ec, T &obj,
                   Handler handler) {
    if (ec) {
      handler(ec);
    } else {
      std::istringstream sin(m_inbound_data);
      try {
        IArchive ia(sin);
        ia >> obj;
      } catch (std::exception &) {
        boost::system::error_code error(boost::asio::error::invalid_argument);
        handler(error);
        return;
      }
      handler(ec);
    }
  }

 private:
  block_connection<Connection> m_block_connection;
  std::string m_inbound_data;
  std::string m_outbound_data;
};

}  // namespace asio
}  // namespace bunsan
