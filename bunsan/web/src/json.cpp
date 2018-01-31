#include <bunsan/web/json.hpp>

#include <bunsan/filesystem/fstream.hpp>

namespace bunsan {
namespace web {
namespace json {

cppcms::json::value load_file(const boost::filesystem::path &path) {
  try {
    bunsan::filesystem::ifstream fin(path);
    cppcms::json::value value;
    int line_number;
    if (!value.load(fin, true, &line_number))
      BOOST_THROW_EXCEPTION(load_file_error()
                            << load_file_error::line_number(line_number));
    fin.close();
    return value;
  } catch (load_file_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(load_file_error() << enable_nested_current());
  }
}

void save_file(const cppcms::json::value &value,
               const boost::filesystem::path &path, const int how) {
  try {
    bunsan::filesystem::ofstream fout(path);
    value.save(fout, how);
    fout.close();
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(save_file_error() << enable_nested_current());
  }
}

}  // namespace json
}  // namespace web
}  // namespace bunsan
