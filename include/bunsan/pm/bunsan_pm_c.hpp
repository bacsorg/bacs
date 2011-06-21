#ifndef BUNSAN_PM_C_HPP
#define BUNSAN_PM_C_HPP

#include <cstdlib>

extern "C" void *bunsan_pm_create(char *error, size_t ebufsize, const char *config);
extern "C" int bunsan_pm_clean(char *error, size_t ebufsize, void *repository);
extern "C" int bunsan_pm_extract(char *error, size_t ebufsize, void *repository, const char *package, const char *dir);
extern "C" int bunsan_pm_remove(char *error, size_t ebufsize, void *repository);

#endif //BUNSAN_PM_C_HPP

