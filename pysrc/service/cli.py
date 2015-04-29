import argparse
import logging.config

from bunsan.broker import connection_pb2
from bunsan.broker.service import consumer
from google.protobuf import text_format


def callback(task, send_status):
    pass


def main():
    parser = argparse.ArgumentParser(description='Worker')
    parser.add_argument('--logging', required=True,
                        help='Logging configuration file')
    parser.add_argument('connection')
    parser.add_argument('constraints')
    args = parser.parse_args()
    logging.config.fileConfig(args.logging)
    connection_parameters = connection_pb2.ConnectionParameters()
    constraints = connection_pb2.Constraints()
    text_format.Parse(args.connection, connection_parameters)
    text_format.Parse(args.constraints, constraints)
    cns = consumer.Consumer(connection_parameters=connection_parameters,
                            constraints=constraints)
    try:
        cns.listen(callback)
    finally:
        cns.close()


if __name__ == '__main__':
    main()
