#!/usr/bin/python3
# -*- coding: utf-8 -*-

from ctypes import *

class BunsanRepository(object):
	def __init__(self, config, bunsan_pm):
		self._ebufsize = 1024
		self._pm = bunsan_pm
		error = create_string_buffer(self._ebufsize)
		self._inst = self._pm.bunsan_pm_create(error,  c_size_t(self._ebufsize),  c_char_p(config.encode('utf8')))
		if not self._inst:
			raise RuntimeError(error.value.decode('utf8'))
	def clean(self):
		error = create_string_buffer(self._ebufsize)
		ret = self._pm.bunsan_pm_clean(error,  c_size_t(self._ebufsize),  self._inst)
		if ret!=0:
			raise RuntimeError(error.value.decode('utf8'))
	def extract(self, package, extract_to):
		error = create_string_buffer(self._ebufsize)
		ret = self._pm.bunsan_pm_extract(error,  c_size_t(self._ebufsize),  self._inst,  c_char_p(package.encode('utf8')),  extract_to.encode('utf8'))
		if ret!=0:
			raise RuntimeError(error.value.decode('utf8'))
	def __del__(self):
		error = create_string_buffer(self._ebufsize)
		ret = self._pm.bunsan_pm_remove(error,  c_size_t(self._ebufsize),  self._inst)
		if ret!=0:
			raise RuntimeError(error.value.decode('utf8'))
