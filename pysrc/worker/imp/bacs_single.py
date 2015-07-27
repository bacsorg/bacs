import base64
import logging
import subprocess

from bunsan.broker import protocol_pb2


_EXECUTABLE = 'bin/bacs_system_single'
_MINUTE = 60
_TIMEOUT = 10 * _MINUTE
_KiB = 1024
_MiB = 1024 * _KiB
_MAX_LINE_SIZE = 50 * _MiB


class SolutionRunner(object):

    def __init__(self, status_sender, data):
        self._logger = logging.getLogger(__name__)
        self._status_sender = status_sender
        self._data = data

    def write_data(self, stdin):
        stdin.write(self._data)
        stdin.close()

    def read_data(self, stdout):
        status = protocol_pb2.Status()
        result = protocol_pb2.Result()
        for line in stdout:
            tokens = line.split()
            if len(tokens) != 2:
                self._logger.warning(
                    'Invalid number of token: expected 2, found %d',
                    len(tokens)
                )
            else:
                command, data = tuple(tokens)
                data = base64.decodebytes(data)
                if command == b'status':
                    status.ParseFromString(data)
                    self._status_sender.send_proto(status)
                elif command == b'result':
                    result.ParseFromString(data)
                else:
                    self._logger.warning('Invalid command: %s', command)
        return result

    def read_log(self, stderr):
        return stderr.read()


class Worker(object):

    def __init__(self, executor):
        self._logger = logging.getLogger(__name__)
        self._executor = executor

    def __call__(self, status_sender, root, data):
        runner = SolutionRunner(status_sender, data)
        with subprocess.Popen(args=[_EXECUTABLE],
                              cwd=root,
                              stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE) as proc:
            status_sender.send('EXECUTING')
            self._executor.submit(runner.write_data, proc.stdin)
            result_future = self._executor.submit(runner.read_data, proc.stdout)
            log_future = self._executor.submit(runner.read_log, proc.stderr)
            ret = proc.wait()
            if ret != 0:
                result = protocol_pb2.Result()
                result.status = protocol_pb2.Result.EXECUTION_ERROR
                result.data = log_future.result()
                status_sender.send('FAIL', code=1)
            else:
                result = result_future.result()
                status_sender.send('DONE')
            return result
