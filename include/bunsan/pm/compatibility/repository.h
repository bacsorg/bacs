#ifndef BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
#define BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif
	typedef size_t size_type;
	typedef char *string;
	typedef const char *cstring;
	int bunsan_pm_repository_create(cstring config, cstring path, bool strip, string error_msg, size_type error_size);
	int bunsan_pm_repository_clean(cstring config, string error_msg, size_type error_size);
	int bunsan_pm_repository_extract(cstring config, cstring package, cstring path, string error_msg, size_type error_size);
#ifdef __cplusplus
}
#endif

#endif //BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

