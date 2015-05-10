import logging
import pika
import threading
import time

from bunsan.broker.service import sender
from bunsan.broker import rabbit_pb2


_RETRY_TIME = 5


class Consumer(object):

    def __init__(self, connection_parameters, constraints):
        self._logger = logging.getLogger(__name__)
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
        self._logger.debug('Opening connection')
        self._connection_parameters = pika.ConnectionParameters(**connect)
        self._connection = None
        self._constraints = constraints
        self._callback = None
        self._thread = None
        self._logger.debug('Created consumer')

    def listen(self, callback):
        """
            Args:
                callback(task, send_status(Status)) -> Result
        """
        self._logger.info('Start asynchronous consuming')
        self._callback = callback
        self._thread = threading.Thread(target=self._start_consuming)
        self._thread.start()

    def listen_and_wait(self, callback):
        """
            Args:
                callback(task, send_status(Status)) -> Result
        """
        self._logger.info('Start synchronous consuming')
        self._callback = callback
        self._start_consuming()

    def wait(self):
        self._thread.join()

    def close(self):
        self._logger.info('Closing connection to RabbitMQ')
        if self._connection is not None:
            self._connection.close()
        if self._thread is not None:
            self._thread.join()

    def _connect(self):
        while self._connection is None:
            try:
                self._logger.info('Connecting to RabbitMQ')
                self._connection = pika.BlockingConnection(
                    self._connection_parameters)
                self._logger.info('Connected to RabbitMQ')
            except pika.exceptions.AMQPConnectionError:
                self._logger.exception(
                    'Unable to connect to RabbitMQ, retrying')
                time.sleep(_RETRY_TIME)

    def _start_consuming(self):
        while True:
            try:
                self._connect()
                channel = self._connection.channel()
                channel.basic_qos(prefetch_count=1)
                self._logger.debug('Start consuming')
                for queue in self._constraints.resource:
                    self._logger.debug('Consuming queue=%s', queue)
                    channel.queue_declare(queue=queue, durable=True)
                    channel.basic_consume(queue=queue,
                                          consumer_callback=self._consume)
                channel.start_consuming()
            except pika.exceptions.AMQPConnectionError:
                self._logger.exception(
                    'Broken connection to RabbitMQ, retrying')
                self._connection = None

    def _consume(self, channel, method, properties, body):
        """Only commit logic, does not throw"""
        try:
            self._do_consume(channel, method, properties, body)
            self._logger.info('Acknowledging: %s',
                              properties.correlation_id)
            channel.basic_ack(delivery_tag=method.delivery_tag)
        except Exception:
            self._logger.info('Not acknowledging: %s',
                              properties.correlation_id)
            channel.basic_nack(delivery_tag=method.delivery_tag,
                               requeue=False)

    def _do_consume(self, channel, method, properties, body):
        """Actual consume implementation, may throw exceptions"""
        self._logger.info('Received task: %s', properties.correlation_id)
        error_sender = sender.ErrorSender(channel, properties)
        rabbit_task = rabbit_pb2.RabbitTask()
        try:
            rabbit_task.ParseFromString(body)
        except Exception as e:
            self._logger.exception('Unable to parse task proto')
            error_sender.sendmsg('Unable to parse task proto: %s', e)
            raise
        status_sender = sender.StatusSender(channel,
                                            rabbit_task.status_queue,
                                            rabbit_task.identifier)
        result_sender = sender.ResultSender(channel,
                                            rabbit_task.result_queue,
                                            rabbit_task.identifier)
        self._logger.debug('Running callback')

        def send_status(status):
            rabbit_status = rabbit_pb2.RabbitStatus()
            rabbit_status.identifier = rabbit_task.identifier
            rabbit_status.status.CopyFrom(status)
            status_sender.send_proto(rabbit_status)
        try:
            result = self._callback(task=rabbit_task.task,
                                    send_status=send_status)
            self._logger.debug('Completed callback')
        except Exception as e:
            self._logger.exception('Unable to complete callback')
            error_sender.sendmsg('Unable to complete callback: %s', e)
            raise
        try:
            rabbit_result = rabbit_pb2.RabbitResult()
            rabbit_result.identifier = rabbit_task.identifier
            rabbit_result.result.CopyFrom(result)
            result_sender.send_proto(rabbit_result)
            self._logger.info('Sent result')
        except Exception as e:
            self._logger.exception('Unable to serialize result proto')
            error_sender.sendmsg('Unable to serialize result proto: %s', e)
            raise

    def __enter__(self):
        return self

    def __exit__(self, type, value, tb):
        self.close()
