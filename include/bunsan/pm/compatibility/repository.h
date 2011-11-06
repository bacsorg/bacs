#ifndef BUNSAN_PM_COMPATIBILITY_REPOSITORY_H
#define BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
	void *bunsan_pm_new(char *error, size_t ebufsize, const char *config);
	int bunsan_pm_clean(char *error, size_t ebufsize, void *repository);
	int bunsan_pm_extract(char *error, size_t ebufsize, void *repository, const char *package, const char *dir);
	int bunsan_pm_create(char *error, size_t ebufsize, void *repository, const char *path, bool strip);
	int bunsan_pm_delete(char *error, size_t ebufsize, void *repository);
#ifdef __cplusplus
}
#endif

#endif //BUNSAN_PM_COMPATIBILITY_REPOSITORY_H

