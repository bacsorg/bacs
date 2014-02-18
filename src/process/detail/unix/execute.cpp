#include <bunsan/process/detail/execute.hpp>

#include "executor.hpp"

#include <bunsan/system_error.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

namespace bunsan{namespace process{namespace detail
{
    typedef boost::error_info<struct tag_pid, pid_t> unix_pid;

    int sync_execute(const context &ctx)
    {
        executor exec_(ctx.executable, ctx.arguments);
        const pid_t pid = ::fork();
        if (pid < 0)
        {
            BOOST_THROW_EXCEPTION(system_error("fork"));
        }
        else if (pid > 0)
        {
            for (;;)
            {
                int stat_loc;
                const pid_t rpid = ::waitpid(pid, &stat_loc, 0);
                if (rpid != pid)
                {
                    BOOST_ASSERT_MSG(rpid < 0, "WNOHANG was not set");
                    if (errno != EINTR)
                    {
                        BOOST_THROW_EXCEPTION(
                            system_error("waitpid") <<
                            unix_pid(pid));
                    }
                }
                else
                {
                    if (WIFEXITED(stat_loc))
                    {
                        return WEXITSTATUS(stat_loc);
                    }
                    else
                    {
                        BOOST_ASSERT(WIFSIGNALED(stat_loc));
                        return -WTERMSIG(stat_loc);
                    }
                }
            }
        }
        else
        {
            BOOST_ASSERT(pid == 0);
            try
            {
                boost::filesystem::current_path(ctx.current_path);
                exec_.exec();
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                std::abort();
            }
            catch (...)
            {
                std::cerr << "Unknown exception." << std::endl;
                std::abort();
            }
        }
    }
}}}
