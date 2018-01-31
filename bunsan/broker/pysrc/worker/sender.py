from bunsan.broker import protocol_pb2


class StatusSender(object):

    def __init__(self, send_status):
        self._send_status = send_status

    def send(self, reason, code=None, data=None):
        status = protocol_pb2.Status()
        status.reason = reason
        if code is not None:
            status.code = code
        if data is not None:
            status.data = data
        self.send_proto(status)

    def send_proto(self, status):
        self._send_status(status)
