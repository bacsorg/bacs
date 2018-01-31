#pragma once

#include <boost/preprocessor/cat.hpp>

/*!
 * Provides macros for module static initializers.
 *
 * \warning Should never be included in header file.
 */

#define BUNSAN_STATIC_INITIALIZER_NAME(NAME) BOOST_PP_CAT(NAME, _initializer)

#ifdef BUNSAN_STATIC_INITIALIZER_GENERATOR
// Build system supports automatic initializer creation.
#define BUNSAN_STATIC_INITIALIZER(NAME, CODE) \
  extern "C" void BUNSAN_STATIC_INITIALIZER_NAME(NAME)() CODE
#else
// No build system support, use C++ facilities.
#define BUNSAN_STATIC_INITIALIZER(NAME, CODE)                    \
  namespace {                                                    \
  struct BUNSAN_STATIC_INITIALIZER_NAME(NAME) {                  \
    BUNSAN_STATIC_INITIALIZER_NAME(NAME)() CODE                  \
  } BOOST_PP_CAT(BUNSAN_STATIC_INITIALIZER_NAME(NAME), _sentry); \
  }

#define BUNSAN_STATIC_INITIALIZER_TEXT(CODE) int N
#endif
