class ExecutionError(RuntimeError):
    pass


class WorkerError(RuntimeError):
    pass


class UnknownWorkerError(WorkerError):
    pass
