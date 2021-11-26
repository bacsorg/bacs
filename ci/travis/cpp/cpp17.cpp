#include <optional>

int main() {
  static_assert(true);
  std::optional<int> x = 0;
  return *x;
}
