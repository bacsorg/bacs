#include <bunsan/process/executor.hpp>

namespace bunsan::process {

class boost_executor : public executor {
 public:
  boost_executor() = default;

  int sync_execute(context ctx) override;
};

}  // namespace bunsan::process
