import locale

from detail cimport *

class RepositoryError(Exception):
    def __init__(self, bytes error_msg, size_type error_size):
        size = _strnlen(error_msg, error_size)
        error = _decode(error_msg[:size])
        super(RepositoryError, self).__init__(error)

cdef size_type _strnlen(cstring s, size_type n) nogil:
    cdef char c
    cdef size_type i
    for i in range(n):
        if s[i]==0:
            return i
    return n

cdef str _getlocale():
    return locale.getlocale()[1]

cdef bytes _encode(str s):
    return s.encode(_getlocale())

cdef str _decode(bytes s):
    return s.decode(_getlocale())

DEF error_size = 2**10

cdef void _bunsan_pm_repository_create(str config, str source, bool strip) except *:
    cdef char error_msg[error_size]
    config_ = _encode(config)
    source_ = _encode(source)
    if 0!=bunsan_pm_repository_create(config_, source_, strip, error_msg, error_size):
        raise RepositoryError(error_msg, error_size)

cdef void _bunsan_pm_repository_clean(str config) except *:
    cdef char error_msg[error_size]
    config_ = _encode(config)
    if 0!=bunsan_pm_repository_clean(config_, error_msg, error_size):
        raise RepositoryError(error_msg, error_size)

cdef void _bunsan_pm_repository_extract(str config, str package, str destination) except *:
    cdef char error_msg[error_size]
    cdef bytes config_ = _encode(config)
    cdef bytes package_ = _encode(package)
    cdef bytes destination_ = _encode(destination)
    if 0!=bunsan_pm_repository_extract(config_, package_, destination_, error_msg, error_size):
        raise RepositoryError(error_msg, error_size)

cdef class Repository(object):
    cdef str _config
    def __init__(self, config):
        self._config = config
    cpdef create(self, str source, bint strip=False):
        _bunsan_pm_repository_create(self._config, source, strip)
    cpdef clean(self):
        _bunsan_pm_repository_clean(self._config)
    cpdef extract(self, str package, str destination):
        _bunsan_pm_repository_extract(self._config, package, destination)

