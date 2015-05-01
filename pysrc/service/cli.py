import argparse
import logging
import logging.config

from bunsan.broker import connection_pb2
from bunsan.broker.service import consumer
from google.protobuf import text_format


def callback(task, send_status):
    print(task)


def main():
    parser = argparse.ArgumentParser(description='Server')
    parser.add_argument('--logging', required=True,
                        help='Logging configuration file')
    parser.add_argument('connection', help='Proto-encoded ConnectionParameters')
    parser.add_argument('constraints', help='Proto-encoded Constraints')
    args = parser.parse_args()
    logging.config.fileConfig(args.logging)
    _logger = logging.getLogger(__name__)
    connection_parameters = connection_pb2.ConnectionParameters()
    constraints = connection_pb2.Constraints()
    text_format.Parse(args.connection, connection_parameters)
    text_format.Parse(args.constraints, constraints)
    _logger.info('Creating consumer')
    cns = consumer.Consumer(connection_parameters=connection_parameters,
                            constraints=constraints)
    try:
        cns.listen(callback)
        cns.wait()
    finally:
        cns.close()


if __name__ == '__main__':
    main()
