import argparse
import logging
import logging.config

from bunsan.broker import connection_pb2
from bunsan.broker.service import consumer
from bunsan.broker.worker import worker
from google.protobuf import text_format


def main():
    parser = argparse.ArgumentParser(description='Server')
    parser.add_argument('--logging', required=True,
                        help='Logging configuration file')
    parser.add_argument('--connection', required=True,
                        help='Proto-encoded ConnectionParameters')
    parser.add_argument('--constraints', required=True,
                        help='Proto-encoded Constraints')
    parser.add_argument('--jobs', type=int, default=1,
                        help='Number of jobs to run in parallel')
    parser.add_argument('--repository-config', required=True,
                        help='Configuration for bunsan::pm::repository')
    parser.add_argument('--tmpdir', default='/tmp',
                        help='Temporary directory for task execution')
    args = parser.parse_args()
    logging.config.fileConfig(args.logging)
    _logger = logging.getLogger(__name__)
    connection_parameters = connection_pb2.ConnectionParameters()
    constraints = connection_pb2.Constraints()
    text_format.Parse(args.connection, connection_parameters)
    text_format.Parse(args.constraints, constraints)
    _logger.info('Creating consumer')
    with consumer.Consumer(connection_parameters=connection_parameters,
                           constraints=constraints) as cns:
        _logger.info('Creating worker')
        with worker.Worker(jobs=args.jobs,
                           tmpdir=args.tmpdir,
                           repository_config=args.repository_config) as wrk:
            cns.listen_and_wait(wrk.callback)


if __name__ == '__main__':
    main()
