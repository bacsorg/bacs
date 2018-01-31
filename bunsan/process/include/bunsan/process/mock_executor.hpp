#include <bunsan/process/executor.hpp>

#include <turtle/mock.hpp>

namespace bunsan {
namespace process {

MOCK_BASE_CLASS(mock_executor, executor) {
  MOCK_METHOD(sync_execute, 1, int(context ctx))
};

class mock_executor_fixture {
 public:
  mock_executor_fixture()
      : executor(std::make_shared<mock_executor>()),
        m_previous_executor(executor::instance()) {
    executor::register_instance(executor);
  }

  ~mock_executor_fixture() { executor::register_instance(m_previous_executor); }

  std::shared_ptr<mock_executor> executor;

 private:
  executor_ptr m_previous_executor;
};

}  // namespace process
}  // namespace bunsan
