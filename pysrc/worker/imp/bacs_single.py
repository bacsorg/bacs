import base64
import logging
import subprocess
import threading
import traceback

from bunsan.broker import protocol_pb2
from bunsan.broker.worker import error


_EXECUTABLE = 'bacs/bacs_system_single'
_MINUTE = 60
_TIMEOUT = 10 * _MINUTE
_KiB = 1024
_MiB = 1024 * _KiB
_MAX_LINE_SIZE = 50 * _MiB


class Worker(object):

    def __init__(self, executor):
        self._logger = logging.getLogger(__name__)
        self._executor = executor

    def __call__(self, status_sender, root, data):
        result = protocol_pb2.Result()
        result.status = protocol_pb2.Result.OK
        result.reason = 'OK'
        result.data = b'data'
        return result  # FIXME stub
        with subprocess.Popen(args=[_EXECUTABLE],
                              cwd=root,
                              stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE) as proc:
            status_sender.send('EXECUTING')
            # TODO
            ret = proc.wait()
            if ret != 0:
                self._send_status('FAIL', code=1)
                raise error.ExecutionError()  # FIXME
            status_sender.send('DONE')
