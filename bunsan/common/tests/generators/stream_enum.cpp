#include <bunsan/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

namespace generator {
enum def_scope { free_, inclass };
const char *def_scope_name[] = {"free_", "inclass"};
enum type { untyped, typed };
const char *type_name[] = {"untyped", "typed"};
enum value { defaulted, initialized };
const char *value_name[] = {"defaulted", "initialized"};
enum item_scope { unscoped, scoped };
const char *item_scope_name[] = {"unscoped", "scoped"};

template <typename T>
std::string ss(const T &obj) {
  return boost::lexical_cast<std::string>(obj);
}
}  // namespace generator

int main(int argc, char *argv[]) {
  BOOST_ASSERT(argc == 1 + 1);
  bunsan::filesystem::ofstream fout(argv[1]);

  std::size_t depth = 0;
  const auto indent = [&]() { fout << std::string(2 * depth, ' '); };
  const auto print = [&](const std::string &s) { fout << s; };
  const auto iprint = [&](const std::string &s) {
    indent();
    print(s);
  };
  const auto println = [&](const std::string &s) {
    print(s);
    fout << '\n';
  };
  const auto iprintln = [&](const std::string &s) {
    iprint(s);
    fout << '\n';
  };

  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
    using namespace generator;

    fout << R"EOF(#define BOOST_TEST_MODULE stream_enum
#include <boost/test/unit_test.hpp>

#include <bunsan/stream_enum.hpp>

#include <boost/lexical_cast.hpp>

#include <type_traits>

#include <cstdint>

BOOST_AUTO_TEST_SUITE(stream_enum)
)EOF";
    for (int ds = free_; ds <= inclass; ++ds) {
      fout << "\nBOOST_AUTO_TEST_SUITE(" << def_scope_name[ds] << ")\n";
      for (int t = untyped; t <= typed; ++t) {
        fout << "\nBOOST_AUTO_TEST_SUITE(" << type_name[t] << ")\n";
        for (int v = defaulted; v <= initialized; ++v) {
          fout << "\nBOOST_AUTO_TEST_SUITE(" << value_name[v] << ")\n";
          for (int is = unscoped; is <= scoped; ++is) {
            fout << "\nBOOST_AUTO_TEST_SUITE(" << item_scope_name[is] << ")\n";
            {
              const std::string underlying_type = "std::uint64_t";
              std::size_t value_1 = 0, value_2 = 1, value_3 = 2;
              const std::string struct_name = "my_struct";
              std::string enum_prefix = "";
              std::string enum_name = "my_enum";
              std::string item_prefix = "";
              println("");
              if (ds == inclass) {
                println("struct " + struct_name + " {");
                ++depth;
              }
              iprint("BUNSAN");
              if (ds == inclass) print("_INCLASS");
              if (t == typed) print("_TYPED");
              print("_STREAM_ENUM");
              if (is == scoped) print("_CLASS");
              if (v == initialized) print("_INITIALIZED");
              print("(" + enum_name + ", ");
              if (t == typed) {
                print(underlying_type + ", ");
              }
              println("(");
              ++depth;
              if (v == initialized) {
                iprintln("(FIRST, 2),");
                value_1 = 2;
                iprintln("(SECOND),");
                value_2 = value_1 + 1;
                iprintln("(THIRD, 5)");
                value_3 = 5;
              } else {
                iprintln("FIRST,");
                value_1 = 0;
                iprintln("SECOND,");
                value_2 = value_1 + 1;
                iprintln("THIRD");
                value_3 = value_2 + 1;
              }
              --depth;
              iprintln("))");
              if (ds == inclass) {
                --depth;
                iprintln("};");
                enum_prefix = struct_name + "::" + enum_prefix;
              }
              enum_name = enum_prefix + enum_name;
              if (is == scoped) {
                item_prefix = enum_name + "::";
              } else {
                item_prefix = enum_prefix;
              }
              println("");
              iprintln("BOOST_AUTO_TEST_CASE(test) {");
              ++depth;
              {
                iprintln("BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(" +
                         item_prefix + "FIRST), \"FIRST\");");
                iprintln("BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(" +
                         item_prefix + "SECOND), \"SECOND\");");
                iprintln("BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(" +
                         item_prefix + "THIRD), \"THIRD\");");
                iprintln(
                    "BOOST_CHECK_THROW(boost::lexical_cast<std::string>(static_"
                    "cast<" +
                    enum_name + ">(100500)),");
                iprintln("                  " + enum_name +
                         "_invalid_enum_representation_error);");
                iprintln("BOOST_CHECK(boost::lexical_cast<" + enum_name +
                         ">(\"FIRST\") == " + item_prefix + "FIRST);");
                iprintln("BOOST_CHECK(boost::lexical_cast<" + enum_name +
                         ">(\"SECOND\") == " + item_prefix + "SECOND);");
                iprintln("BOOST_CHECK(boost::lexical_cast<" + enum_name +
                         ">(\"THIRD\") == " + item_prefix + "THIRD);");
                iprintln("BOOST_CHECK_THROW(boost::lexical_cast<" + enum_name +
                         ">(\"unknown\"),");
                iprintln("                  " + enum_name +
                         "_invalid_string_representation_error);");
                iprintln("BOOST_CHECK_EQUAL(static_cast<int>(" + item_prefix +
                         "FIRST), " + ss(value_1) + ");");
                iprintln("BOOST_CHECK_EQUAL(static_cast<int>(" + item_prefix +
                         "SECOND), " + ss(value_2) + ");");
                iprintln("BOOST_CHECK_EQUAL(static_cast<int>(" + item_prefix +
                         "THIRD), " + ss(value_3) + ");");
                if (t == typed) {
                  iprintln("static_assert(std::is_same<std::underlying_type<" +
                           enum_name + ">::type, " + underlying_type +
                           ">::value, \"\");");
                }
              }
              --depth;
              iprintln("}");
            }
            fout << "\nBOOST_AUTO_TEST_SUITE_END()  // " << item_scope_name[is]
                 << '\n';
          }
          fout << "\nBOOST_AUTO_TEST_SUITE_END()  // " << value_name[v] << '\n';
        }
        fout << "\nBOOST_AUTO_TEST_SUITE_END()  // " << type_name[t] << '\n';
      }
      fout << "\nBOOST_AUTO_TEST_SUITE_END()  // " << def_scope_name[ds]
           << '\n';
    }
    fout << "\nBOOST_AUTO_TEST_SUITE_END()  // "
         << "stream_enum" << '\n';
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
  fout.close();
}
