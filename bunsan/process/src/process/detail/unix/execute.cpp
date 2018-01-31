#include <bunsan/process/detail/execute.hpp>

#include "error.hpp"
#include "executor.hpp"

#include <bunsan/process/file/handle.hpp>
#include <bunsan/process/signal.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/noncopyable.hpp>
#include <boost/variant/static_visitor.hpp>

#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

namespace bunsan {
namespace process {
namespace detail {

int sync_execute(context ctx) {
  executor exec_(ctx.executable, ctx.arguments);

  const pid_t pid = ::fork();
  if (pid < 0) {
    BOOST_THROW_EXCEPTION(system_error("fork"));
  } else if (pid > 0) {
    for (;;) {
      int stat_loc;
      const pid_t rpid = ::waitpid(pid, &stat_loc, 0);
      if (rpid != pid) {
        BOOST_ASSERT_MSG(rpid < 0, "WNOHANG was not set");
        if (errno != EINTR) {
          BOOST_THROW_EXCEPTION(system_error("waitpid")
                                << system_error::pid(pid));
        }
      } else {
        if (WIFEXITED(stat_loc)) {
          return WEXITSTATUS(stat_loc);
        } else {
          BOOST_ASSERT(WIFSIGNALED(stat_loc));
          return -WTERMSIG(stat_loc);
        }
      }
    }
  } else {
    BOOST_ASSERT(pid == 0);
    try {
      reset_signals();
      boost::filesystem::current_path(ctx.current_path);

      if (*ctx.stdin_file != STDIN_FILENO)
        ctx.stdin_file = ctx.stdin_file.dup2(*file::handle::std_input());
      ctx.stdin_file.set_inheritable();
      if (*ctx.stdout_file != STDOUT_FILENO)
        ctx.stdout_file = ctx.stdout_file.dup2(*file::handle::std_output());
      ctx.stdout_file.set_inheritable();
      if (*ctx.stderr_file != STDERR_FILENO)
        ctx.stderr_file = ctx.stderr_file.dup2(*file::handle::std_error());
      ctx.stderr_file.set_inheritable();

      exec_.exec();
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      std::abort();
    } catch (...) {
      std::cerr << "Unknown exception." << std::endl;
      std::abort();
    }
  }
}

}  // namespace detail
}  // namespace process
}  // namespace bunsan
