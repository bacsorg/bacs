#pragma once

#include "server_fixture.hpp"

#include <bunsan/curl/easy.hpp>

struct easy_fixture : server_fixture {
  bunsan::curl::easy easy;
};
