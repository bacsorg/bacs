#include <bunsan/pm/compatibility/repository.h>

#include <bunsan/pm/compatibility/repository.hpp>

#include <boost/assert.hpp>

#include <cstring>

namespace {
template <typename F>
int wrap_cpp(F &&f, string error_msg, size_type error_size) noexcept {
  try {
    f();
    return 0;
  } catch (std::exception &e) {
    std::strncpy(error_msg, e.what(), error_size);
    return 1;
  } catch (...) {
    std::strncpy(error_msg, "[unknown]", error_size);
    return -1;
  }
}

template <typename... MemFnArgs, typename... Args>
int wrap_repository(
    void (bunsan::pm::compatibility::repository::*mem_fn)(MemFnArgs...),
    cstring config, string error_msg, size_type error_size,
    Args &&... args) noexcept {
  return wrap_cpp(
      [&] {
        bunsan::pm::compatibility::repository repo(config);
        (repo.*mem_fn)(std::forward<Args>(args)...);
      },
      error_msg, error_size);
}
}  // namespace

int bunsan_pm_create(cstring config, cstring source, bool strip,
                     string error_msg, size_type error_size) {
  return ::wrap_repository(&bunsan::pm::compatibility::repository::create,
                           config, error_msg, error_size, source, strip);
}

int bunsan_pm_clean_cache(cstring config, string error_msg,
                          size_type error_size) {
  return ::wrap_repository(&bunsan::pm::compatibility::repository::clean_cache,
                           config, error_msg, error_size);
}

int bunsan_pm_extract(cstring config, cstring package, cstring destination,
                      string error_msg, size_type error_size) {
  return ::wrap_repository(&bunsan::pm::compatibility::repository::extract,
                           config, error_msg, error_size, package, destination);
}

bunsan_pm_repository bunsan_pm_repository_new(cstring config, string error_msg,
                                              size_type error_size) {
  bunsan::pm::compatibility::repository *repo = nullptr;
  wrap_cpp([&] { repo = new bunsan::pm::compatibility::repository(config); },
           error_msg, error_size);
  return repo;
}

void bunsan_pm_repository_free(bunsan_pm_repository repo) {
  delete static_cast<bunsan::pm::compatibility::repository *>(repo);
}

int bunsan_pm_repository_create_recursively(bunsan_pm_repository repo,
                                            cstring root, bool strip,
                                            string error_msg,
                                            size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->create_recursively(root, strip); }, error_msg,
                  error_size);
}

int bunsan_pm_repository_create(bunsan_pm_repository repo, cstring path,
                                bool strip, string error_msg,
                                size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->create(path, strip); }, error_msg, error_size);
}

int bunsan_pm_repository_extract(bunsan_pm_repository repo, cstring package,
                                 cstring destination, string error_msg,
                                 size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->extract(package, destination); }, error_msg,
                  error_size);
}

int bunsan_pm_repository_install(bunsan_pm_repository repo, cstring package,
                                 cstring destination, string error_msg,
                                 size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->install(package, destination); }, error_msg,
                  error_size);
}

int bunsan_pm_repository_force_update(bunsan_pm_repository repo,
                                      cstring package, cstring destination,
                                      string error_msg, size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->update(package, destination); }, error_msg,
                  error_size);
}

int bunsan_pm_repository_update(bunsan_pm_repository repo, cstring package,
                                cstring destination, time_t lifetime,
                                string error_msg, size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->update(package, destination, lifetime); }, error_msg,
                  error_size);
}

int bunsan_pm_repository_need_update(bunsan_pm_repository repo, cstring package,
                                     cstring destination, time_t lifetime,
                                     bool *need, string error_msg,
                                     size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  BOOST_ASSERT(need);
  return wrap_cpp(
      [&] { *need = r->need_update(package, destination, lifetime); },
      error_msg, error_size);
}

int bunsan_pm_repository_clean_cache(bunsan_pm_repository repo,
                                     string error_msg, size_type error_size) {
  const auto r = static_cast<bunsan::pm::compatibility::repository *>(repo);
  BOOST_ASSERT(r);
  return wrap_cpp([&] { r->clean_cache(); }, error_msg, error_size);
}
