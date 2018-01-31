#pragma once

#include <boost/test/test_tools.hpp>

#define BUNSAN_IF_CHECK(exp) \
  BOOST_CHECK(exp);          \
  if (exp)
#define BUNSAN_IF_CHECK_MESSAGE(P, M) \
  BOOST_CHECK_MESSAGE(P, M);          \
  if (exp)
#define BUNSAN_IF_CHECK_EQUAL(L, R) \
  BOOST_CHECK_EQUAL(L, R);          \
  if (L == R)
#define BUNSAN_IF_CHECK_NE(L, R) \
  BOOST_CHECK_NE(L, R);          \
  if (L != R)
#define BUNSAN_IF_CHECK_LT(L, R) \
  BOOST_CHECK_LT(L, R);          \
  if (L < R)
#define BUNSAN_IF_CHECK_LE(L, R) \
  BOOST_CHECK_LE(L, R);          \
  if (L <= R)
#define BUNSAN_IF_CHECK_GT(L, R) \
  BOOST_CHECK_GT(L, R);          \
  if (L > R)
#define BUNSAN_IF_CHECK_GE(L, R) \
  BOOST_CHECK_GE(L, R);          \
  if (L >= R)
#define BUNSAN_IF_CHECK_BITWISE_EQUAL(L, R) \
  BOOST_CHECK_BITWISE_EQUAL(L, R);          \
  if (L != R)
