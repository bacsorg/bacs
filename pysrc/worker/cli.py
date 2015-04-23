import argparse
import logging
import pika

from bunsan.broker import worker_pb2
from bunsan.broker.worker import sender
from bunsan.broker.rabbitmq import common_pb2 as rabbitmq_pb2


_logger = logging.getLogger('bunsan.broker.worker.cli')


class Runner(object):

    def __init__(self, channel, task):
        self._status = sender.StatusSender(channel=channel,
                                           queue=task.status_queue,
                                           identifier=task.identifier)
        self._result = sender.ResultSender(channel=channel,
                                           queue=task.result_queue,
                                           identifier=task.identifier)
        self._task = task.task
        self._status.send_status(0, 'acknowledged')

    def run(self):
        _logger.error('Not implemented')


def callback(channel, method, properties, body):
    _logger.info('Received')
    context = sender.Context(channel, properties)
    task = rabbitmq_pb2.RabbitTask()
    try:
        task.ParseFromString(body)
    except Exception as e:
        _logger.exception('ParseFromString', e)
        context.send_result('INVALID_PROTO', "Unable to parse proto", str(e))
        channel.basic_ack(delivery_tag=method.delivery_tag)
    runner = Runner(channel, task)
    _logger.info('Running')
    runner.run()
    _logger.info('Completed')
    channel.basic_ack(delivery_tag=method.delivery_tag)


def main():
    parser = argparse.ArgumentParser(description='Worker')
    parser.add_argument('connection')
    args = parser.parse_args()
    connection_parameters = worker_pb2.ConnectionParameters()
    connection_parameters.ParseFromString(args.connection)
    credentials = pika.credentials.PlainCredentials(
        username=connection_parameters.credentials.username,
        password=connection_parameters.credentials.password)
    connection = pika.BlockingConnection(pika.ConnectionParameters(
        host=connection_parameters.connection.host,
        port=connection_parameters.connection.port,
        virtual_host=connection_parameters.connection.virtual_host,
        credentials=credentials))
    channel = connection.channel()
    channel.basic_qos(prefetch_count=1)
    for queue in connection.constraints:
        channel.queue_declare(queue=queue, durable=True)
        channel.basic_consume(callback, queue=queue)
    _logger.info('Start consuming')
    channel.start_consuming()


if __name__ == '__main__':
    main()
