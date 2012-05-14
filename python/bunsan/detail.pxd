from libcpp cimport bool

cdef extern from "bunsan/pm/compatibility/repository.h":
    ctypedef size_t size_type
    ctypedef char *string
    ctypedef char *cstring "const char *"
    int bunsan_pm_repository_create(cstring config, cstring source, bool strip, string error_msg, size_type error_size) nogil
    int bunsan_pm_repository_clean(cstring config, string error_msg, size_type error_size) nogil
    int bunsan_pm_repository_extract(cstring config, cstring package, cstring destination, string error_msg, size_type error_size) nogil

