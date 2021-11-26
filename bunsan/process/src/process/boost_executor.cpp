#include <bunsan/process/boost_executor.hpp>

#include <bunsan/process/path.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/tempfile.hpp>

#include <boost/assert.hpp>
#include <boost/io/quoted.hpp>  // for logging
#include <boost/noncopyable.hpp>
#include <boost/process.hpp>
#include <boost/variant/multivisitors.hpp>
#include <boost/variant/static_visitor.hpp>

#include <sstream>  // for logging
#include <type_traits>

#ifdef BOOST_POSIX_API
#include <sys/types.h>
#include <sys/wait.h>
#endif

namespace bunsan::process {

namespace {
struct child_visitor : boost::static_visitor<boost::process::child>,
                       private boost::noncopyable {
  boost::filesystem::path exe, start_dir;
  std::vector<std::string> args;
  boost::process::environment env;
  tempfile tmp;

  template <typename StdIn, typename StdOut, typename StdErr>
  boost::process::child operator()(const StdIn stdin_data,
                                   const StdOut &stdout_data,
                                   const StdErr &stderr_data) {
    return boost::process::child(
        // FIXME https://github.com/boostorg/process/issues/32
        boost::process::exe = exe.native(),
        boost::process::start_dir = start_dir, boost::process::args = args,
        boost::process::env = env,
        apply_stdin(stdin_data), apply_stdout(stdout_data),
        apply_stderr(stderr_data));
  }

  template <typename T>
  auto apply_stdin(const T &stdin_data) {
    if constexpr (std::is_same_v<T, file::do_default_type>) {
      return boost::process::std_in.null();
    } else if constexpr (std::is_same_v<T, file::inherit_type>) {
      return boost::process::std_in = stdin;
    } else if constexpr (std::is_same_v<T, file::suppress_type>) {
      return boost::process::std_in.null();
    } else if constexpr (std::is_same_v<T, std::string>) {
      tmp = tempfile::regular_file_in_tempdir();
      bunsan::filesystem::ofstream fout(tmp.path());
      BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
        fout << stdin_data;
      } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
      fout.close();
      return boost::process::std_in = tmp.path();
    } else {
      static_assert(std::is_same_v<T, boost::filesystem::path>);
      return boost::process::std_in = stdin_data;
    }
  }

  template <typename T>
  auto apply_stdout(const T &stdout_data) {
    if constexpr (std::is_same_v<T, file::do_default_type>) {
      return boost::process::std_out = stdout;
    } else if constexpr (std::is_same_v<T, file::inherit_type>) {
      return boost::process::std_out = stdout;
    } else if constexpr (std::is_same_v<T, file::suppress_type>) {
      return boost::process::std_out.null();
    } else {
      static_assert(std::is_same_v<T, boost::filesystem::path>);
      return boost::process::std_out = stdout_data;
    }
  }

  template <typename T>
  auto apply_stderr(const T &stderr_data) {
    if constexpr (std::is_same_v<T, file::do_default_type>) {
      return boost::process::std_err = stderr;
    } else if constexpr (std::is_same_v<T, file::inherit_type>) {
      return boost::process::std_err = stderr;
    } else if constexpr (std::is_same_v<T, file::suppress_type>) {
      return boost::process::std_err.null();
    } else if constexpr (std::is_same_v<T, file::redirect_to_stdout_type>) {
      return boost::process::std_err = stdout;
    } else {
      static_assert(std::is_same_v<T, boost::filesystem::path>);
      return boost::process::std_err = stderr_data;
    }
  }
};
}  // namespace

int boost_executor::sync_execute(context ctx) {
  ctx.build();
  BUNSAN_LOG_TRACE << "Attempt to execute " << ctx.executable() << " in "
                   << ctx.current_path() << (ctx.use_path() ? " " : " without")
                   << " using path";
  child_visitor child_vis;
  if (ctx.use_path()) {
    child_vis.exe = find_executable_in_path(ctx.executable());
  } else {
    child_vis.exe = ctx.executable();
  }

  child_vis.start_dir = ctx.current_path();
  child_vis.args = ctx.arguments();
  if (!child_vis.args.empty()) {
    // boost::process does not support setting argv[0]
    child_vis.args.erase(child_vis.args.begin());
  }
  for (const auto &entry : ctx.environment()) {
    child_vis.env.emplace(entry.first, entry.second);
  }

  {  // begin logging section
    std::ostringstream sout;
    sout << "Executing " << child_vis.exe << " in " << child_vis.start_dir
         << " with arguments = [";
    for (std::size_t i = 0; i < child_vis.args.size(); ++i) {
      if (i) sout << ", ";
      sout << boost::io::quoted(child_vis.args[i]);
    }
    sout << ']';
    BUNSAN_LOG_TRACE << sout.str();
  }  // end logging section

  boost::process::child child = boost::apply_visitor(
      child_vis, ctx.stdin_data(), ctx.stdout_data(), ctx.stderr_data());
  child.wait();
  return child.exit_code();
}

}  // namespace bunsan::process
