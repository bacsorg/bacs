import logging
import pika
import threading

from bunsan.broker.service import sender
from bunsan.broker import rabbit_pb2


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
        self._connection = pika.BlockingConnection(
            pika.ConnectionParameters(**connect))
        self._channel = self._connection.channel()
        self._channel.basic_qos(prefetch_count=1)
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
        self._connection.close()
        if self._thread is not None:
            self._thread.join()

    def _start_consuming(self):
        self._logger.debug('Start consuming')
        for queue in self._constraints.resource:
            self._logger.debug('Consuming queue=%s', queue)
            self._channel.queue_declare(queue=queue, durable=True)
            self._channel.basic_consume(queue=queue,
                                        consumer_callback=self._consume)
        self._channel.start_consuming()

    def _consume(self, channel, method, properties, body):
        self._logger.info('Received task')
        error_sender = sender.ErrorSender(channel, properties)
        rabbit_task = rabbit_pb2.RabbitTask()
        try:
            rabbit_task.ParseFromString(body)
        except Exception as e:
            self._logger.exception('ParseFromString', e)
            error_sender.sendmsg('Unable to parse task proto: {}', e)
            channel.basic_nack(delivery_tag=method.delivery_tag, requeue=False)
            return
        status_sender = sender.StatusSender(channel,
                                            rabbit_task.status_queue,
                                            rabbit_task.identifier)
        result_sender = sender.ResultSender(channel,
                                            rabbit_task.result_queue,
                                            rabbit_task.identifier)
        self._logger.debug('Running callback')
        try:
            result = self._callback(task=rabbit_task.task,
                                    send_status=status_sender.send_proto)
            self._logger.debug('Completed callback')
        except Exception as e:
            error_sender.sendmsg('Unable to complete callback: %s', e)
            self._logger.error('Sent error: %s', e)
            channel.basic_nack(delivery_tag=method.delivery_tag, requeue=False)
            return
        try:
            rabbit_result = rabbit_pb2.RabbitResult()
            rabbit_result.identifier = rabbit_task.identifier
            rabbit_result.result.CopyFrom(result)
            result_sender.send_proto(rabbit_result)
            self._logger.info('Sent result')
            channel.basic_ack(delivery_tag=method.delivery_tag)
        except Exception as e:
            error_sender.sendmsg('Unable to serialize result proto: %s', e)
            self._logger.error('Sent error: %s', e)
            channel.basic_nack(delivery_tag=method.delivery_tag, requeue=False)
            return
