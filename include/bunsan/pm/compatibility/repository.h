#ifndef BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
#define BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#else
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#error only C99 or higher versions are supported
#endif
#endif

typedef size_t size_type;
typedef char *string;
typedef const char *cstring;
typedef void *bunsan_pm_repository;

int bunsan_pm_create(cstring config, cstring path, bool strip, string error_msg,
                     size_type error_size);
int bunsan_pm_clean_cache(cstring config, string error_msg,
                          size_type error_size);
int bunsan_pm_extract(cstring config, cstring package, cstring destination,
                      string error_msg, size_type error_size);

bunsan_pm_repository bunsan_pm_repository_new(cstring config, string error_msg,
                                              size_type error_size);
void bunsan_pm_repository_free(bunsan_pm_repository repo);

int bunsan_pm_repository_create(bunsan_pm_repository repo, cstring path,
                                bool strip, string error_msg,
                                size_type error_size);
int bunsan_pm_repository_create_recursively(bunsan_pm_repository repo,
                                            cstring root, bool strip,
                                            string error_msg,
                                            size_type error_size);
int bunsan_pm_repository_extract(bunsan_pm_repository repo, cstring package,
                                 cstring destination, string error_msg,
                                 size_type error_size);
int bunsan_pm_repository_install(bunsan_pm_repository repo, cstring package,
                                 cstring destination, string error_msg,
                                 size_type error_size);
int bunsan_pm_repository_force_update(bunsan_pm_repository repo,
                                      cstring package, cstring destination,
                                      string error_msg, size_type error_size);
int bunsan_pm_repository_update(bunsan_pm_repository repo, cstring package,
                                cstring destination, time_t lifetime,
                                string error_msg, size_type error_size);
int bunsan_pm_repository_need_update(bunsan_pm_repository repo, cstring package,
                                     cstring destination, time_t lifetime,
                                     bool *need, string error_msg,
                                     size_type error_size);
int bunsan_pm_repository_clean_cache(bunsan_pm_repository repo,
                                     string error_msg, size_type error_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
