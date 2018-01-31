#include <bunsan/filesystem/operations.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/error.hpp>
#include <bunsan/filesystem/fstream.hpp>

#include <deque>

namespace bunsan {
namespace filesystem {

void reset_dir(const boost::filesystem::path &dir) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    if (boost::filesystem::exists(dir)) boost::filesystem::remove_all(dir);
    boost::filesystem::create_directory(dir);
  } BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(
       error::path(dir), boost::filesystem::filesystem_error)
}

boost::filesystem::path keep_in_root(const boost::filesystem::path &path,
                                     const boost::filesystem::path &root) {
  std::deque<boost::filesystem::path> stack;
  for (const boost::filesystem::path &i : path) {
    if (i == "..") {
      if (!stack.empty()) stack.pop_back();
    } else if (i != ".") {
      stack.push_back(i);
    }
  }
  boost::filesystem::path stripped;
  for (const boost::filesystem::path &i : stack) stripped /= i;
  return root / stripped;
}

#define BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()                 \
  BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(                \
      error::source_path(from) << error::destination_path(to), \
      boost::filesystem::filesystem_error)

void copy_tree(const boost::filesystem::path &from,
               const boost::filesystem::path &to) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    copy_tree(from, to, copy_option::fail_if_exists);
  } BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
}

void copy_tree(const boost::filesystem::path &from,
               const boost::filesystem::path &to,
               boost::system::error_code &ec) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    copy_tree(from, to, copy_option::fail_if_exists, ec);
  } BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
}

void copy_tree(const boost::filesystem::path &from,
               const boost::filesystem::path &to,
               const BOOST_SCOPED_ENUM(copy_option) option) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    boost::system::error_code ec;
    copy_tree(from, to, option, ec);
    if (ec)
      BOOST_FILESYSTEM_THROW(
          boost::filesystem::filesystem_error("", from, to, ec));
  } BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
}

void copy_tree(const boost::filesystem::path &from,
               const boost::filesystem::path &to,
               const BOOST_SCOPED_ENUM(copy_option) option,
               boost::system::error_code &ec) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    switch (option) {
      case copy_option::fail_if_exists:
        if (boost::filesystem::exists(to))
          ec.assign(EEXIST, boost::system::system_category());
        break;
      case copy_option::overwrite_if_exists:
        boost::filesystem::remove_all(to, ec);
        break;
    }
    if (ec) return;
    const boost::filesystem::file_status s(
        boost::filesystem::symlink_status(from, ec));
    if (ec) return;
    boost::filesystem::copy(from, to, ec);
    if (ec) return;
    if (is_directory(from, ec) && !ec) {
      for (boost::filesystem::directory_iterator i(from), end; !ec && i != end;
           ++i) {
        copy_tree(i->path(), to / i->path().filename(), ec);
      }
    }
  } BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
}

std::string read_data(const boost::filesystem::path &path,
                      const std::size_t size_limit) {
  try {
    ifstream fin(path);
    std::string data;
    char buf[BUFSIZ];
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
      do {
        fin.read(buf, BUFSIZ);
        if (data.size() + fin.gcount() > size_limit) {
          BOOST_THROW_EXCEPTION(
              read_data_size_limit_exceeded_error()
              << read_data_size_limit_exceeded_error::path(path)
              << read_data_size_limit_exceeded_error::size_limit(size_limit));
        }
        data.append(buf, fin.gcount());
      } while (fin);
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
    fin.close();
    return data;
  } catch (read_data_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(read_data_error() << read_data_error::path(path)
                                            << enable_nested_current());
  }
}

void write_data(const boost::filesystem::path &path, const std::string &data) {
  try {
    ofstream fout(path);
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
      fout.write(data.data(), data.size());
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(write_data_error() << write_data_error::path(path));
  }
}

}  // namespace filesystem
}  // namespace bunsan
