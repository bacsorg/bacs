#include <bunsan/process/executor.hpp>

namespace bunsan::process {

class native_executor : public executor {
 public:
  native_executor() = default;

  int sync_execute(context ctx) override;

 private:
  int sync_execute_impl(const context &ctx);
};

}  // namespace bunsan::process
