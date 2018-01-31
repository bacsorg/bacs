#include <bunsan/process/execute.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/tempfile.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cstdlib>

namespace bunsan {
namespace process {

namespace {
template <typename Error>
const Error &provide_output(const Error &error,
                            const std::string *const output = nullptr) {
  if (output) error << typename Error::output(*output);
  return error;
}

void check_sync_execute_with_optional_output(context ctx, bool with_output) {
  try {
    if (with_output) {
      const char *const inherit = std::getenv("BUNSAN_PROCESS_INHERIT");
      if (inherit) with_output = false;
    }
    std::string output;
    const int exit_status =
        with_output ? sync_execute_with_output(ctx, output) : sync_execute(ctx);

    if (exit_status) {
      BOOST_THROW_EXCEPTION(provide_output(
          non_zero_exit_status_error()
              << non_zero_exit_status_error::exit_status(exit_status)
              << non_zero_exit_status_error::context(ctx),
          with_output ? &output : nullptr));
    }
  } catch (check_sync_execute_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(check_sync_execute_error()
                          << check_sync_execute_error::context(ctx)
                          << enable_nested_current());
  }
}
}  // namespace

int sync_execute_with_output(context ctx, std::string &output) {
  try {
    tempfile output_tmp;
    output_tmp = tempfile::regular_file_in_tempdir();
    ctx.stdout_file(output_tmp.path());
    ctx.stderr_redirect_to_stdout();

    const int exit_status = sync_execute(std::move(ctx));

    output.clear();
    constexpr std::size_t buf_size = 4096;
    constexpr std::size_t mebibyte = 1024 * 1024;
    // note: actual size may be greater by buf_size
    constexpr std::size_t output_max_size = 10 * mebibyte;
    filesystem::ifstream fin(output_tmp.path());
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
      char buf[buf_size];
      do {
        fin.read(buf, buf_size);
        output.append(buf, fin.gcount());
      } while (fin && output.size() < output_max_size);
    }
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
    fin.close();

    boost::algorithm::trim(output);

    return exit_status;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(sync_execute_with_output_error()
                          << sync_execute_with_output_error::context(ctx)
                          << enable_nested_current());
  }
}

void check_sync_execute(context ctx) {
  check_sync_execute_with_optional_output(std::move(ctx), false);
}

void check_sync_execute_with_output(context ctx) {
  check_sync_execute_with_optional_output(std::move(ctx), true);
}

}  // namespace process
}  // namespace bunsan
