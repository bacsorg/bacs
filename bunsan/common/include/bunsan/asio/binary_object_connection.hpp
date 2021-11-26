#pragma once

#include <bunsan/asio/basic_object_connection.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace bunsan::asio {

template <typename Connection>
using binary_object_connection =
    basic_object_connection<boost::archive::binary_iarchive,
                            boost::archive::binary_oarchive, Connection>;

}  // namespace bunsan::asio
