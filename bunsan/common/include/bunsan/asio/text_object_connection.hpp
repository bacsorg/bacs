#pragma once

#include <bunsan/asio/basic_object_connection.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace bunsan::asio {

template <typename Connection>
using text_object_connection =
    basic_object_connection<boost::archive::text_iarchive,
                            boost::archive::text_oarchive, Connection>;

}  // namespace bunsan::asio
