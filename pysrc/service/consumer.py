import logging
import pika
import threading

from bunsan.broker.service import sender
from bunsan.broker import rabbit_pb2


_logger = logging.getLogger('bunsan.broker.service.consumer')


class Consumer(object):

    def __init__(self, connection_parameters, constraints):
        connect = dict()
        if connection_parameters.host:
            connect['host'] = connection_parameters.host
        if connection_parameters.port:
            connect['port'] = connection_parameters.port
        if connection_parameters.virtual_host:
            connect['virtual_host'] = connection_parameters.virtual_host
        if connection_parameters.HasField('credentials'):
            connect['credentials'] = pika.credentials.PlainCredentials(
                username=connection_parameters.credentials.username,
                password=connection_parameters.credentials.password)
        _logger.debug('Opening connection')
        self._connection = pika.BlockingConnection(
            pika.ConnectionParameters(**connect))
        self._channel = self._connection.channel()
        self._channel.basic_qos(prefetch_count=1)
        for queue in constraints.resource:
            self._channel.queue_declare(queue=queue, durable=True)
            self._channel.basic_consume(self._consume, queue=queue)
        self._callback = None

    def listen(self, callback):
        """
            Args:
                callback(task, send_status(Status)) -> Result
        """
        _logger.info('Start consuming')
        self._callback = callback
        self._thread = threading.Thread(target=self._channel.start_consuming)
        self._thread.start()

    def close(self):
        _logger.info('Closing connection to RabbitMQ')
        self._connection.close()
        self._thread.join()

    def _consume(self, channel, method, properties, body):
        _logger.info('Received')
        error_sender = sender.ErrorSender(channel, properties)
        task = rabbit_pb2.RabbitTask()
        try:
            task.ParseFromString(body)
        except Exception as e:
            _logger.exception('ParseFromString', e)
            error_sender.send('Unable to parse proto: {}'.format(e))
            channel.basic_ack(delivery_tag=method.delivery_tag)
        status_sender = sender.StatusSender(channel,
                                            task.status_queue,
                                            task.identifier)
        result_sender = sender.ResultSender(channel,
                                            task.result_queue,
                                            task.identifier)
        _logger.info('Running')
        result = self._callback(task=task.task,
                                send_status=status_sender.send_proto)
        result_sender.send_proto(result)
        _logger.info('Completed')
        channel.basic_ack(delivery_tag=method.delivery_tag)
