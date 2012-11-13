#include "bunsan/pm/error.hpp"

bunsan::pm::error::error(const std::string &message_):
    bunsan::error(message_) {}

bunsan::pm::invalid_configuration::invalid_configuration(const std::string &message_):
    bunsan::pm::error(message_) {}
