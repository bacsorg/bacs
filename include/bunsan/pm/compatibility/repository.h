#ifndef BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
#define BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

#include <stddef.h>

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

int bunsan_pm_repository_create(cstring config, cstring path, bool strip,
                                string error_msg, size_type error_size);

int bunsan_pm_repository_clean_cache(cstring config, string error_msg,
                                     size_type error_size);

int bunsan_pm_repository_extract(cstring config, cstring package, cstring path,
                                 string error_msg, size_type error_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
