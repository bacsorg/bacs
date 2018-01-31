#include <boost/system/api_config.hpp>

#if defined(BOOST_POSIX_API)
#define BOOST_TEST_MODULE asio
#include <boost/test/unit_test.hpp>

#include <bunsan/asio/binary_object_connection.hpp>
#include <bunsan/asio/block_connection.hpp>
#include <bunsan/asio/buffer_connection.hpp>
#include <bunsan/asio/line_connection.hpp>
#include <bunsan/asio/queued_writer.hpp>
#include <bunsan/asio/text_object_connection.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

BOOST_AUTO_TEST_SUITE(asio)

namespace ba = bunsan::asio;

struct local_socket_fixture {
  using socket = boost::asio::local::stream_protocol::socket;

  struct socket_pair {
    explicit socket_pair(boost::asio::io_service &io_service)
        : first(io_service), second(io_service) {}

    socket first, second;
  };

  socket_pair &pair(const std::size_t index) {
    if (index >= pairs.size()) pairs.resize(index + 1);
    if (!pairs[index]) {
      pairs[index] = std::make_unique<socket_pair>(io_service);
      boost::asio::local::connect_pair(pairs[index]->first,
                                       pairs[index]->second);
    }
    return *pairs[index];
  }

  boost::asio::io_service io_service;
  std::vector<std::unique_ptr<socket_pair>> pairs;
};

struct socket_pair_fixture : local_socket_fixture {
  socket_pair_fixture() : socket1(pair(0).first), socket2(pair(0).second) {}

  socket &socket1, &socket2;
};

BOOST_FIXTURE_TEST_SUITE(serialization, socket_pair_fixture)

BOOST_AUTO_TEST_CASE(block_connection) {
  bool bc1_completed = false;
  bool bc2_completed = false;
  std::string bc1_data, bc2_data;
  ba::block_connection<socket> bc1(socket1), bc2(socket2);
  BOOST_CHECK_EQUAL(&bc1.get_io_service(), &io_service);
  BOOST_CHECK_EQUAL(&bc2.get_io_service(), &io_service);
  bc1.async_write("first request", [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    bc1.async_read(bc1_data, [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      BOOST_CHECK_EQUAL(bc1_data, "first response");
      bc1.async_write(
          "second request", [&](const boost::system::error_code &ec) {
            BOOST_REQUIRE(!ec);
            bc1.async_read(bc1_data, [&](const boost::system::error_code &ec) {
              BOOST_REQUIRE(!ec);
              BOOST_CHECK_EQUAL(bc1_data, "second response");
              bc1.close();
              bc1_completed = true;
            });
          });
    });
  });
  bc2.async_read(bc2_data, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    BOOST_CHECK_EQUAL(bc2_data, "first request");
    bc2.async_write("first response", [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      bc2.async_read(bc2_data, [&](const boost::system::error_code &ec) {
        BOOST_REQUIRE(!ec);
        BOOST_CHECK_EQUAL(bc2_data, "second request");
        bc2.async_write(
            "second response", [&](const boost::system::error_code &ec) {
              BOOST_REQUIRE(!ec);
              bc2.async_read(bc2_data,
                             [&](const boost::system::error_code &ec) {
                               BOOST_REQUIRE_EQUAL(ec, boost::asio::error::eof);
                               bc2_completed = true;
                             });
            });
      });
    });
  });
  io_service.run();
  BOOST_CHECK(bc1_completed);
  BOOST_CHECK(bc2_completed);
}

using object_connections =
    boost::mpl::list<ba::binary_object_connection<socket_pair_fixture::socket>,
                     ba::text_object_connection<socket_pair_fixture::socket>>;

template <typename T>
class message {
 public:
  message() : m_data{} {}
  message(const T &data) : m_data{data} {}

  message(const message &) = default;
  message(message &&) = default;
  message &operator=(const message &) = default;
  message &operator=(message &&) = default;

  message &operator=(const T &data_) {
    m_data = data_;
    return *this;
  }

 private:
  friend class boost::serialization::access;

  template <typename P>
  friend std::ostream &operator<<(std::ostream &out, const message<P> &msg) {
    return out << msg.m_data;
  }

  template <typename P>
  friend bool operator==(const message<T> &msg, const P &data) {
    return msg.m_data == data;
  }

  template <typename P>
  friend bool operator==(const P &data, const message<P> &msg) {
    return msg.m_data == data;
  }

  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::make_nvp("data", m_data);
  }

  T m_data;
};

template <typename ObjectConnection>
struct session : boost::asio::coroutine {
  explicit session(socket_pair_fixture::socket &socket_)
      : oc(std::make_shared<ObjectConnection>(socket_)),
        msg(std::make_shared<message<int>>()) {}

  void check_completed() {
    BOOST_REQUIRE(completed);
    BOOST_CHECK(*completed);
  }

  std::shared_ptr<ObjectConnection> oc;
  std::shared_ptr<message<int>> msg;
  std::shared_ptr<bool> completed{std::make_shared<bool>(false)};
};

template <typename ObjectConnection>
struct server_session : session<ObjectConnection> {
  using session<ObjectConnection>::session;
  using session<ObjectConnection>::oc;
  using session<ObjectConnection>::msg;

#include <boost/asio/yield.hpp>
  void operator()(
      const boost::system::error_code &ec = boost::system::error_code{}) {
    BOOST_REQUIRE(!ec);

    reenter(this) {
      *msg = 10;
      yield oc->async_write(*msg, *this);

      yield oc->async_read(*msg, *this);
      BOOST_CHECK_EQUAL(*msg, 100);

      *msg = 20;
      yield oc->async_write(*msg, *this);

      yield oc->async_read(*msg, *this);
      BOOST_CHECK_EQUAL(*msg, 200);

      oc->close();
      *this->completed = true;
    }
  }
#include <boost/asio/unyield.hpp>
};

template <typename ObjectConnection>
struct client_session : session<ObjectConnection> {
  using session<ObjectConnection>::session;
  using session<ObjectConnection>::oc;
  using session<ObjectConnection>::msg;

#include <boost/asio/yield.hpp>
  void operator()(
      const boost::system::error_code &ec = boost::system::error_code{}) {
    reenter(this) {
      BOOST_REQUIRE(!ec);

      yield oc->async_read(*msg, *this);
      BOOST_REQUIRE(!ec);
      BOOST_CHECK_EQUAL(*msg, 10);

      *msg = 100;
      yield oc->async_write(*msg, *this);
      BOOST_REQUIRE(!ec);

      yield oc->async_read(*msg, *this);
      BOOST_REQUIRE(!ec);
      BOOST_CHECK_EQUAL(*msg, 20);

      *msg = 200;
      yield oc->async_write(*msg, *this);
      BOOST_REQUIRE(!ec);

      yield oc->async_read(*msg, *this);
      BOOST_REQUIRE_EQUAL(ec, boost::asio::error::eof);
      *this->completed = true;
    }
  }
#include <boost/asio/unyield.hpp>
};

BOOST_AUTO_TEST_CASE_TEMPLATE(object_connection_coroutine, ObjectConnection,
                              object_connections) {
  server_session<ObjectConnection> server(socket1);
  client_session<ObjectConnection> client(socket2);

  BOOST_CHECK_EQUAL(&server.oc->get_io_service(), &io_service);
  BOOST_CHECK_EQUAL(&client.oc->get_io_service(), &io_service);

  server();
  client();

  io_service.run();
  server.check_completed();
  client.check_completed();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(object_connection_spawn, ObjectConnection,
                              object_connections) {
  bool oc1_completed = false;
  bool oc2_completed = false;
  boost::asio::spawn(io_service, [&](boost::asio::yield_context yield) {
    ObjectConnection oc(socket1);
    message<int> msg;

    msg = 10;
    oc.async_write(msg, yield);

    oc.async_read(msg, yield);
    BOOST_CHECK_EQUAL(msg, 100);

    msg = 20;
    oc.async_write(msg, yield);

    oc.async_read(msg, yield);
    BOOST_CHECK_EQUAL(msg, 200);

    oc.close();
    oc1_completed = true;
  });

  boost::asio::spawn(io_service, [&](boost::asio::yield_context yield) {
    ObjectConnection oc(socket2);
    message<int> msg;

    oc.async_read(msg, yield);
    BOOST_CHECK_EQUAL(msg, 10);

    msg = 100;
    oc.async_write(msg, yield);

    oc.async_read(msg, yield);
    BOOST_CHECK_EQUAL(msg, 20);

    msg = 200;
    oc.async_write(msg, yield);

    boost::system::error_code ec;
    oc.async_read(msg, yield[ec]);
    BOOST_REQUIRE_EQUAL(ec, boost::asio::error::eof);
    oc2_completed = true;
  });

  io_service.run();
  BOOST_CHECK(oc1_completed);
  BOOST_CHECK(oc2_completed);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(object_connection, ObjectConnection,
                              object_connections) {
  bool oc1_completed = false;
  bool oc2_completed = false;
  message<int> oc1_msg, oc2_msg;
  ObjectConnection oc1(socket1), oc2(socket2);
  oc1_msg = 10;
  oc1.async_write(oc1_msg, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    oc1.async_read(oc1_msg, [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      BOOST_CHECK_EQUAL(oc1_msg, 100);
      oc1_msg = 20;
      oc1.async_write(oc1_msg, [&](const boost::system::error_code &ec) {
        BOOST_REQUIRE(!ec);
        oc1.async_read(oc1_msg, [&](const boost::system::error_code &ec) {
          BOOST_REQUIRE(!ec);
          BOOST_CHECK_EQUAL(oc1_msg, 200);
          oc1.close();
          oc1_completed = true;
        });
      });
    });
  });
  oc2.async_read(oc2_msg, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    BOOST_CHECK_EQUAL(oc2_msg, 10);
    oc2_msg = 100;
    oc2.async_write(oc2_msg, [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      oc2.async_read(oc2_msg, [&](const boost::system::error_code &ec) {
        BOOST_REQUIRE(!ec);
        BOOST_CHECK_EQUAL(oc2_msg, 20);
        oc2_msg = 200;
        oc2.async_write(oc2_msg, [&](const boost::system::error_code &ec) {
          BOOST_REQUIRE(!ec);
          oc2.async_read(oc2_msg, [&](const boost::system::error_code &ec) {
            BOOST_REQUIRE_EQUAL(ec, boost::asio::error::eof);
            oc2_completed = true;
          });
        });
      });
    });
  });
  io_service.run();
  BOOST_CHECK(oc1_completed);
  BOOST_CHECK(oc2_completed);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(object_connection_serialize_error,
                              ObjectConnection, object_connections) {
  const std::string nan_int = "NaN-int";
  bool completed = false;
  message<int> oc1_msg, oc2_msg;
  ba::block_connection<socket_pair_fixture::socket> bc1(socket1);
  ObjectConnection oc2(socket2);
  bc1.async_write(nan_int, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    bc1.close();
    oc2_msg = 11;
    oc2.async_read(oc2_msg, [&](const boost::system::error_code &ec) {
      BOOST_CHECK(ec == boost::asio::error::invalid_argument);
      completed = true;
    });
  });
  io_service.run();
  BOOST_CHECK(completed);
}

BOOST_AUTO_TEST_SUITE_END()  // serialization

BOOST_FIXTURE_TEST_SUITE(queued_writer, socket_pair_fixture)

struct data {
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(thread);
    ar & BOOST_SERIALIZATION_NVP(iteration);
  }

  std::size_t thread = 0;
  std::size_t iteration = 0;
};

constexpr std::size_t worker_number = 10;
constexpr std::size_t thread_number = 4;
constexpr std::size_t thread_iterations = 20;
constexpr std::size_t iterations = thread_number * thread_iterations;

BOOST_AUTO_TEST_CASE(test) {
  ba::text_object_connection<socket_pair_fixture::socket> oc1(socket1),
      oc2(socket2);
  ba::queued_writer<data, decltype(oc1)> writer(oc1);

  boost::thread_group threads;

  boost::barrier start_barrier(thread_number);
  boost::barrier close_barrier(thread_number);

  data msg;
  std::vector<std::size_t> thread_iteration(thread_number);
  std::size_t iteration = 0;

  boost::function<void()> read;

  const auto handle_read = [&](const boost::system::error_code &ec) {
    if (ec) {
      BOOST_REQUIRE_EQUAL(iteration, iterations);
      BOOST_REQUIRE(ec == boost::asio::error::eof);
      return;
    }

    BOOST_REQUIRE_LT(msg.thread, thread_number);
    BOOST_REQUIRE_EQUAL(msg.iteration, thread_iteration[msg.thread]++);
    ++iteration;

    read();
  };

  read = [&]() { oc2.async_read(msg, handle_read); };

  read();

  for (std::size_t i = 0; i < worker_number; ++i)
    threads.create_thread(
        boost::bind(&boost::asio::io_service::run, &io_service));

  for (std::size_t thread = 0; thread < thread_number; ++thread) {
    threads.create_thread([&, thread]() {
      start_barrier.wait();

      data d;
      d.thread = thread;
      for (std::size_t i = 0; i < thread_iterations; ++i) {
        d.iteration = i;
        writer.write(d);
      }

      if (close_barrier.wait()) writer.close();
    });
  }
  threads.join_all();
}

BOOST_AUTO_TEST_SUITE_END()  // queued_writer

BOOST_FIXTURE_TEST_SUITE(buffer_connection, local_socket_fixture)

constexpr std::size_t worker_number = 10;

const std::vector<std::string> message = {
    "hello",   "world", "this", "is", "some",   "asynchronous",
    "message", "just",  "list", "of", "strings"};

std::size_t message_size() {
  std::size_t size = 0;
  for (const auto &msg : message) size += msg.size();
  return size;
}

template <std::size_t message_number>
void writer(socket_pair_fixture::socket &socket) {
  for (std::size_t i = 0; i < message_number; ++i) {
    for (const std::string &msg : message)
      boost::asio::write(socket, boost::asio::buffer(msg));
  }
  socket.close();
}

template <std::size_t message_number>
void reader(socket_pair_fixture::socket &socket) {
  std::vector<char> buffer;
  for (std::size_t i = 0; i < message_number; ++i) {
    for (const std::string &msg : message) {
      buffer.resize(msg.size());
      const std::size_t size =
          boost::asio::read(socket, boost::asio::buffer(buffer));
      BOOST_ASSERT(size == buffer.size());
      const std::string actual_msg(buffer.data(), buffer.size());
      BOOST_ASSERT(actual_msg == msg);
    }
  }
  buffer.resize(1);
  boost::system::error_code ec;
  const std::size_t size =
      boost::asio::read(socket, boost::asio::buffer(buffer), ec);
  BOOST_ASSERT(ec == boost::asio::error::eof);
  BOOST_ASSERT(size == 0);
}

using message_numbers =
    boost::mpl::list<boost::mpl::size_t<1000>, boost::mpl::size_t<0>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(test, MessageNumber, message_numbers) {
  constexpr std::size_t message_number = MessageNumber::value;

  boost::thread_group threads;

  std::size_t read_size = 0, write_size = 0;
  bool eof = false;

  ba::buffer_connection<socket, socket> buffer(
      pair(0).second, pair(1).first,
      [&](const boost::system::error_code &ec, const std::size_t size) {
        read_size += size;
        if (ec) {
          BOOST_ASSERT(ec == boost::asio::error::eof);
          BOOST_ASSERT(!eof);
          eof = true;
        }
      },
      [&](const boost::system::error_code &ec, const std::size_t size) {
        write_size += size;
        BOOST_ASSERT(!ec);
      });
  buffer.start();

  for (std::size_t i = 0; i < worker_number; ++i)
    threads.create_thread(
        boost::bind(&boost::asio::io_service::run, &io_service));

  threads.create_thread(
      boost::bind(writer<message_number>, boost::ref(pair(0).first)));

  threads.create_thread(
      boost::bind(reader<message_number>, boost::ref(pair(1).second)));

  threads.join_all();

  BOOST_CHECK(eof);
  const std::size_t expected_size = message_number * message_size();
  BOOST_CHECK_EQUAL(write_size, expected_size);
  BOOST_CHECK_EQUAL(read_size, expected_size);
}

BOOST_AUTO_TEST_SUITE_END()  // buffer_connection

BOOST_FIXTURE_TEST_SUITE(line_connection, socket_pair_fixture)

BOOST_AUTO_TEST_CASE(test) {
  bool sink_completed = false;
  bool source_completed = false;
  ba::line_connection<socket> sink(socket1), source(socket2);

  std::string msg, buf;

  msg = "hello, world";
  sink.async_write(msg, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);

    msg = "speak C++";
    sink.async_write(msg, [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      sink.close();
      sink_completed = true;
    });
  });

  source.async_read(buf, [&](const boost::system::error_code &ec) {
    BOOST_REQUIRE(!ec);
    BOOST_CHECK_EQUAL(buf, "hello, world");

    source.async_read(buf, [&](const boost::system::error_code &ec) {
      BOOST_REQUIRE(!ec);
      BOOST_CHECK_EQUAL(buf, "speak C++");

      source.async_read(buf, [&](const boost::system::error_code &ec) {
        BOOST_REQUIRE(ec == boost::asio::error::eof);
        source_completed = true;
      });
    });
  });

  io_service.run();
  BOOST_CHECK(sink_completed);
  BOOST_CHECK(source_completed);
}

BOOST_AUTO_TEST_SUITE_END()  // line_connection

BOOST_AUTO_TEST_SUITE_END()  // asio
#else
int main() {}
#endif
