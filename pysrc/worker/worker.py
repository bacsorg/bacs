from concurrent import futures
import importlib
import logging
import tempfile
import traceback

import bunsan.pm

from bunsan.broker.worker import sender
from bunsan.broker import protocol_pb2


_THREADS_PER_JOB = 3


class ExecutionError(RuntimeError):
    pass


class Worker(object):

    def __init__(self, jobs, tmpdir, repository_config):
        self._logger = logging.getLogger(__name__)
        self._tmpdir = tmpdir
        self._repository = bunsan.pm.Repository(repository_config)
        self._executor = futures.ThreadPoolExecutor(
            max_workers=_THREADS_PER_JOB * jobs)

    def _extract(self, status_sender, tmpdir, package):
        status_sender.send('EXTRACTING')
        self._logger.debug('Extracting to %s...', tmpdir)
        self._repository.extract(package, tmpdir)
        status_sender.send('EXTRACTED')

    def _execute(self, status_sender, worker, tmpdir, data):
        self._logger.debug('Running')
        imp = importlib.import_module(package='bunsan.broker.worker.imp',
                                      name=worker)
        worker = imp.Worker(executor=self._executor)
        return worker(status_sender=status_sender,
                      root=tmpdir,
                      data=data)

    def callback(self, task, send_status):
        self._logger.debug('task = %s', task)
        result = protocol_pb2.Result()
        status_sender = sender.StatusSender(send_status)
        try:
            with tempfile.TemporaryDirectory(dir=self._tmpdir) as tmpdir:
                self._extract(status_sender=status_sender,
                              tmpdir=tmpdir,
                              package=task.package)
                result = self._execute(status_sender=status_sender,
                                       worker=task.worker,
                                       tmpdir=tmpdir,
                                       data=task.data)
            send_status('DONE')

        except ExecutionError:
            result.status = protocol_pb2.Result.EXECUTION_ERROR
            result.reason = traceback.format_exc()
        except bunsan.pm.Error:
            result.status = protocol_pb2.Result.PACKAGE_ERROR
            result.reason = traceback.format_exc()
        except Exception:
            result.status = protocol_pb2.Result.ERROR
            result.reason = traceback.format_exc()
        return result

    def cleanup(self):
        self._executor.shutdown()

    def __enter__(self):
        return self

    def __exit__(self, type, value, tb):
        self.cleanup()
