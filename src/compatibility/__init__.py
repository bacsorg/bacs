#!/usr/bin/python3

from bunsan.pm import _pm


class Error(Exception):

    def __init__(self, **kwargs):
        super(Error, self).__init__(kwargs)


class CreateError(Error):
    pass


class ExtractError(Error):
    pass


class InitializeCacheError(Error):
    pass


class CleanCacheError(Error):
    pass


class Repository(object):

    def __init__(self, config_path):
        self._impl = _pm.Repository(config_path)

    def create(self, path, strip=False):
        try:
            self._impl.create(path, strip)
        except Exception as e:
            raise CreateError(path=path, strip=strip) from e

    def extract(self, package, path):
        try:
            self._impl.extract(package, path)
        except Exception as e:
            raise ExtractError(package=package, path=path) from e

    @staticmethod
    def initialize_cache(config):
        _pm.Repository.initialize_cache(config_)

    def clean_cache(self):
        try:
            self._impl.clean_cache()
        except Exception as e:
            raise CleanCacheError() from e
