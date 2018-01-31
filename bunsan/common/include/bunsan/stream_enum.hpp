#pragma once

#include <bunsan/error.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/size.hpp>

#include <istream>
#include <ostream>
#include <string>

#include <cstdint>

namespace bunsan {
namespace stream_enum {

/// Base exception class for stream_enum errors.
struct invalid_value_error : virtual error {};

/// String representation is invalid.
struct invalid_string_representation_error : virtual invalid_value_error {
  using string_value = boost::error_info<struct tag_string_value, std::string>;
};

/// Enum representation is invalid.
struct invalid_enum_representation_error : virtual invalid_value_error {
  using enum_value = boost::error_info<struct tag_enum_value, std::intmax_t>;
};

}  // namespace stream_enum
}  // namespace bunsan

/*!
 * \brief Macro repeats a macro for each element in a tuple.
 *
 * macro(r, data, item) for each item in tuple
 */
#define BOOST_PP_TUPLE_FOR_EACH(macro, data, tuple) \
  BOOST_PP_SEQ_FOR_EACH(macro, data, BOOST_PP_TUPLE_TO_SEQ(tuple))

// ******************************** SITEM ******************************** //
// item name
#define BUNSAN_STREAM_ENUM_ITEM_NAME_DEFAULT(ITEM) ITEM

#define BUNSAN_STREAM_ENUM_ITEM_NAME_INITIALIZED(ITEM_VALUE) \
  BOOST_PP_TUPLE_ELEM(0, ITEM_VALUE)

// item
#define BUNSAN_STREAM_ENUM_ITEM_DEFAULT(ITEM) ITEM

#define BUNSAN_STREAM_ENUM_ITEM_INITIALIZED_1(ITEM) BOOST_PP_TUPLE_ELEM(0, ITEM)
#define BUNSAN_STREAM_ENUM_ITEM_INITIALIZED_2(ITEM_VALUE) \
  BOOST_PP_TUPLE_ELEM(0, ITEM_VALUE) = BOOST_PP_TUPLE_ELEM(1, ITEM_VALUE)

#define BUNSAN_STREAM_ENUM_ITEM_INITIALIZED(ITEM)    \
  BOOST_PP_CAT(BUNSAN_STREAM_ENUM_ITEM_INITIALIZED_, \
               BOOST_PP_TUPLE_SIZE(ITEM))(ITEM)

// qualified item
#define BUNSAN_STREAM_ENUM_QITEM_DEFAULT(ENUM, ITEM) ITEM

#define BUNSAN_STREAM_ENUM_QITEM_CLASS(ENUM, ITEM) ENUM::ITEM

// sitem = (raw item name, qualified item name, declaration item name, string)
#define BUNSAN_STREAM_ENUM_SITEM_CREATE(ENUM, CLASS_TYPE, ITEM, DECL_TYPE)   \
  ((BOOST_PP_CAT(BUNSAN_STREAM_ENUM_ITEM_NAME_, DECL_TYPE)(ITEM),            \
    BOOST_PP_CAT(BUNSAN_STREAM_ENUM_QITEM_, CLASS_TYPE)(                     \
        ENUM, BOOST_PP_CAT(BUNSAN_STREAM_ENUM_ITEM_NAME_, DECL_TYPE)(ITEM)), \
    BOOST_PP_CAT(BUNSAN_STREAM_ENUM_ITEM_, DECL_TYPE)(ITEM),                 \
    BOOST_PP_STRINGIZE(                                                      \
        BOOST_PP_CAT(BUNSAN_STREAM_ENUM_ITEM_NAME_, DECL_TYPE)(ITEM))))

#define BUNSAN_STREAM_ENUM_SITEMS_CREATE_PART(r, ENUM_CLASS_DECL, ITEM)    \
  BUNSAN_STREAM_ENUM_SITEM_CREATE(BOOST_PP_TUPLE_ELEM(0, ENUM_CLASS_DECL), \
                                  BOOST_PP_TUPLE_ELEM(1, ENUM_CLASS_DECL), \
                                  ITEM,                                    \
                                  BOOST_PP_TUPLE_ELEM(2, ENUM_CLASS_DECL))

#define BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS_TYPE, ITEMS, DECL_TYPE) \
  BOOST_PP_SEQ_TO_TUPLE(                                                     \
      BOOST_PP_TUPLE_FOR_EACH(BUNSAN_STREAM_ENUM_SITEMS_CREATE_PART,         \
                              (ENUM, CLASS_TYPE, DECL_TYPE), ITEMS))

#define BUNSAN_STREAM_ENUM_SITEM_NAME(SITEM) BOOST_PP_TUPLE_ELEM(0, SITEM)
#define BUNSAN_STREAM_ENUM_SITEM_QNAME(SITEM) BOOST_PP_TUPLE_ELEM(1, SITEM)
#define BUNSAN_STREAM_ENUM_SITEM_DECLNAME(SITEM) BOOST_PP_TUPLE_ELEM(2, SITEM)
#define BUNSAN_STREAM_ENUM_SITEM_SNAME(SITEM) BOOST_PP_TUPLE_ELEM(3, SITEM)

// ******************************** SITEM ******************************** //

// ******************************** ERRORS ******************************** //

/// Error name for exception derived from
/// ::bunsan::stream_enum::invalid_value_error.
#define BUNSAN_STREAM_ENUM_ERROR(ENUM) BOOST_PP_CAT(ENUM, _invalid_value_error)

/// Error name for exception derived from
/// ::bunsan::stream_enum::invalid_string_representation_error.
#define BUNSAN_STREAM_ENUM_ERROR_STRING(ENUM) \
  BOOST_PP_CAT(ENUM, _invalid_string_representation_error)

/// Error name for exception derived from
/// ::bunsan::stream_enum::invalid_enum_representation_error.
#define BUNSAN_STREAM_ENUM_ERROR_ENUM(ENUM) \
  BOOST_PP_CAT(ENUM, _invalid_enum_representation_error)

/*!
 * \brief Define exceptions derived from appropriate
 * in ::bunsan::stream_enum namespace.
 *
 * \param ENUM enum name
 */
#define BUNSAN_STREAM_ENUM_ERRORS(ENUM)                                        \
  struct BUNSAN_STREAM_ENUM_ERROR(ENUM)                                        \
      : virtual ::bunsan::stream_enum::invalid_value_error {};                 \
  struct BUNSAN_STREAM_ENUM_ERROR_STRING(ENUM)                                 \
      : virtual BUNSAN_STREAM_ENUM_ERROR(ENUM),                                \
        virtual ::bunsan::stream_enum::invalid_string_representation_error {}; \
  struct BUNSAN_STREAM_ENUM_ERROR_ENUM(ENUM)                                   \
      : virtual BUNSAN_STREAM_ENUM_ERROR(ENUM),                                \
        virtual ::bunsan::stream_enum::invalid_enum_representation_error {};

// ******************************** ERRORS ******************************** //

// *************************** IMPLEMENTATION  *************************** //

#define BUNSAN_STREAM_ENUM_STREAM_FREE inline

#define BUNSAN_STREAM_ENUM_STREAM_INCLASS friend inline

#define BUNSAN_STREAM_ENUM_ISTREAM_PART(r, data, SITEM) \
  if (val == BUNSAN_STREAM_ENUM_SITEM_SNAME(SITEM))     \
    item = BUNSAN_STREAM_ENUM_SITEM_QNAME(SITEM);       \
  else

/// Define std::istream &operator>>(...).
#define BUNSAN_STREAM_ENUM_ISTREAM(SCOPE, ENUM, SITEMS)                        \
  BUNSAN_STREAM_ENUM_STREAM_##SCOPE std::istream &operator>>(std::istream &in, \
                                                             ENUM &item) {     \
    std::string val;                                                           \
    in >> val;                                                                 \
    BOOST_PP_TUPLE_FOR_EACH(BUNSAN_STREAM_ENUM_ISTREAM_PART, UNUSED, SITEMS)   \
    BOOST_THROW_EXCEPTION(                                                     \
        BUNSAN_STREAM_ENUM_ERROR_STRING(ENUM)() << ::bunsan::stream_enum::     \
            invalid_string_representation_error::string_value(val));           \
    return in;                                                                 \
  }

#define BUNSAN_STREAM_ENUM_OSTREAM_PART(r, data, SITEM) \
  case BUNSAN_STREAM_ENUM_SITEM_QNAME(SITEM):           \
    out << BUNSAN_STREAM_ENUM_SITEM_SNAME(SITEM);       \
    break;

/// Define std::ostream &operator<<(...).
#define BUNSAN_STREAM_ENUM_OSTREAM(SCOPE, ENUM, SITEMS)                        \
  BUNSAN_STREAM_ENUM_STREAM_##SCOPE std::ostream &operator<<(                  \
      std::ostream &out, const ENUM &item) {                                   \
    switch (item) {                                                            \
      BOOST_PP_TUPLE_FOR_EACH(BUNSAN_STREAM_ENUM_OSTREAM_PART, UNUSED, SITEMS) \
      default:                                                                 \
        BOOST_THROW_EXCEPTION(                                                 \
            BUNSAN_STREAM_ENUM_ERROR_ENUM(ENUM)() << ::bunsan::stream_enum::   \
                invalid_enum_representation_error::enum_value(static_cast<     \
                    ::bunsan::stream_enum::invalid_enum_representation_error:: \
                        enum_value::value_type>(item)));                       \
    }                                                                          \
    return out;                                                                \
  }

#define BUNSAN_STREAM_ENUM_SITEM_DECLNAME_ARG(r, data, SITEM) \
  (BUNSAN_STREAM_ENUM_SITEM_DECLNAME(SITEM))

#define BUNSAN_STREAM_ENUM_HEADER_NAME_DEFAULT(ENUM) enum ENUM

#define BUNSAN_STREAM_ENUM_HEADER_NAME_CLASS(ENUM) enum class ENUM

#define BUNSAN_STREAM_ENUM_HEADER_DEFAULT(ENUM, CLASS_TYPE, TYPE) \
  BUNSAN_STREAM_ENUM_HEADER_NAME_##CLASS_TYPE(ENUM)

#define BUNSAN_STREAM_ENUM_HEADER_TYPED(ENUM, CLASS_TYPE, TYPE) \
  BUNSAN_STREAM_ENUM_HEADER_NAME_##CLASS_TYPE(ENUM) : TYPE

/*!
 * \brief Define enum with operator<<() and operator>>() defined.
 *
 * This enum is suitable for boost::lexical_cast<NAME>(std::string)
 * and boost::lexical_cast<std::string>(NAME).
 *
 * \param SCOPE where enum is defined: DEFAULT or INCLASS
 * \param ENUM enum name
 * \param CLASS_TYPE DEFAULT for old-style enum and CLASS for scoped enum
 * \param TYPED_TYPE TYPED to override underlying type DEFAULT otherwise
 * \param TYPE if TYPED_TYPE is TYPED used as underlying type otherwise ignored
 * \param SITEMS tuple of sitem structures
 */
#define BUNSAN_STREAM_ENUM_IMPLEMENTATION(SCOPE, ENUM, CLASS_TYPE, TYPED_TYPE, \
                                          TYPE, SITEMS)                        \
  BUNSAN_STREAM_ENUM_HEADER_##TYPED_TYPE(                                      \
      ENUM, CLASS_TYPE, TYPE){BOOST_PP_SEQ_ENUM(BOOST_PP_TUPLE_FOR_EACH(       \
      BUNSAN_STREAM_ENUM_SITEM_DECLNAME_ARG, UNUSED, SITEMS))};                \
  BUNSAN_STREAM_ENUM_ERRORS(ENUM)                                              \
  BUNSAN_STREAM_ENUM_ISTREAM(SCOPE, ENUM, SITEMS)                              \
  BUNSAN_STREAM_ENUM_OSTREAM(SCOPE, ENUM, SITEMS)

// *************************** IMPLEMENTATION  *************************** //

// ******************************** PUBLIC ******************************** //

/*!
 * \note naming:
 * BUNSAN[_INCLASS][_TYPED]_STREAM_ENUM[_CLASS][_INITIALIZED](ENUM[, TYPE],
 *ITEMS)
 * \param INCLASS if declared inside class (or struct) scope FREE otherwise
 * \param TYPED argument after enum name is enum underlying type
 * \param CLASS if enum is scoped
 * \param INITIALIZED to override enum values,
 * (item_name) or (item_name, item_value) should be used
 *
 * Examples:
 *
 * \code{.cpp}
 * BUNSAN_STREAM_ENUM(my_enum, (
 *   my_value_1,
 *   my_value_2
 * ))
 *
 * enum my_enum {
 *   my_value_1,
 *   my_value_2
 * };
 * \endcode
 *
 * \code{.cpp}
 * struct my_struct
 * {
 *   BUNSAN_INCLASS_STREAM_ENUM(my_enum, (
 *     my_value_1,
 *     my_value_2
 *   ))
 * };
 *
 * struct my_struct {
 *   enum my_enum {
 *     my_value_1,
 *     my_value_2
 *   };
 * };
 * \endcode
 *
 * \code{.cpp}
 * BUNSAN_STREAM_ENUM_INITIALIZED(my_enum, (
 *   (my_value_1),
 *   (my_value_2, 10)
 *   (my_value_3)
 * ))
 *
 * enum my_enum {
 *   my_value_1,
 *   my_value_2 = 10,
 *   my_value_3
 * };
 * \endcode
 *
 * \code{.cpp}
 * BUNSAN_TYPED_STREAM_ENUM(my_enum, char, (
 *   my_value_1,
 *   my_value_2
 * ))
 *
 * enum my_enum: char {
 *   my_value_1,
 *   my_value_2
 * };
 * \endcode
 */

#define BUNSAN_STREAM_ENUM(ENUM, ITEMS)     \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(        \
      FREE, ENUM, DEFAULT, DEFAULT, UNUSED, \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_TYPED_STREAM_ENUM(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                \
      FREE, ENUM, DEFAULT, TYPED, TYPE,             \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_STREAM_ENUM_INITIALIZED(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                \
      FREE, ENUM, DEFAULT, DEFAULT, UNUSED,         \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_TYPED_STREAM_ENUM_INITIALIZED(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                            \
      FREE, ENUM, DEFAULT, TYPED, TYPE,                         \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_STREAM_ENUM(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(            \
      INCLASS, ENUM, DEFAULT, DEFAULT, UNUSED,  \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_TYPED_STREAM_ENUM(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                        \
      INCLASS, ENUM, DEFAULT, TYPED, TYPE,                  \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_STREAM_ENUM_INITIALIZED(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                        \
      INCLASS, ENUM, DEFAULT, DEFAULT, UNUSED,              \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_TYPED_STREAM_ENUM_INITIALIZED(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                                    \
      INCLASS, ENUM, DEFAULT, TYPED, TYPE,                              \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, DEFAULT, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_STREAM_ENUM_CLASS(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(          \
      FREE, ENUM, CLASS, DEFAULT, UNUSED,     \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_TYPED_STREAM_ENUM_CLASS(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                      \
      FREE, ENUM, CLASS, TYPED, TYPE,                     \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_STREAM_ENUM_CLASS_INITIALIZED(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                      \
      FREE, ENUM, CLASS, DEFAULT, UNUSED,                 \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                                  \
      FREE, ENUM, CLASS, TYPED, TYPE,                                 \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_STREAM_ENUM_CLASS(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                  \
      INCLASS, ENUM, CLASS, DEFAULT, UNUSED,          \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_TYPED_STREAM_ENUM_CLASS(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                              \
      INCLASS, ENUM, CLASS, TYPED, TYPE,                          \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, DEFAULT))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_STREAM_ENUM_CLASS_INITIALIZED(ENUM, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                              \
      INCLASS, ENUM, CLASS, DEFAULT, UNUSED,                      \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, INITIALIZED))

/// \copydoc BUNSAN_STREAM_ENUM
#define BUNSAN_INCLASS_TYPED_STREAM_ENUM_CLASS_INITIALIZED(ENUM, TYPE, ITEMS) \
  BUNSAN_STREAM_ENUM_IMPLEMENTATION(                                          \
      INCLASS, ENUM, CLASS, TYPED, TYPE,                                      \
      BUNSAN_STREAM_ENUM_SITEMS_CREATE(ENUM, CLASS, ITEMS, INITIALIZED))

// ******************************** PUBLIC ******************************** //
