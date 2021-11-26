#include <bunsan/process/signal.hpp>

#include <signal.h>

namespace bunsan::process {

void reset_signals() {
  // clear parent's sigprocmask
  sigset_t sset;
  sigemptyset(&sset);
  pthread_sigmask(SIG_SETMASK, &sset, nullptr);
}

}  // namespace bunsan::process
