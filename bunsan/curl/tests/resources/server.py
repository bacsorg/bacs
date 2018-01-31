#!/usr/bin/python3

from http.server import *
import subprocess
import sys
import threading
import time


class Handler(BaseHTTPRequestHandler):

    def _ok_headers(self, *headers):
        self.send_response(200)
        for header in headers:
            self.send_header(*header)
        self.end_headers()

    def _error(self, *args, **kwargs):
        self.send_error(*args, **kwargs)

    def _404(self):
        self._error(404)

    big = b'big data stub\n' * 10**6

    def do_GET(self):
        if self.path == "/hello":
            self._ok_headers()
            self.wfile.write(b"Hello, world!")
        elif self.path == "/sleep":
            try:
                time.sleep(1)
                self._ok_headers()
                self.wfile.write(b'DONE')
            except BrokenPipeError:
                self.log_message('"{} {} {}" broken pipe'.format(
                    self.command,
                    self.path,
                    self.request_version
                ))
        elif self.path == "/big":
            self._ok_headers(
                ('Content-Length', str(len(self.big)))
            )
            self.wfile.write(self.big)
        elif self.path == "/header":
            self._ok_headers()
            self.wfile.write(self.headers['X-cURL-Test'].encode('ascii'))
        elif self.path == "/response":
            response_code = int(self.headers['X-cURL-Response-Code'])
            response_reason = self.headers.get('X-cURL-Response-Reason')
            self.send_response(response_code, response_reason)
            self.end_headers()
        else:
            self._404()

    def do_POST(self):
        if self.path == "/echo":
            self._ok_headers()
            if 'Content-Length' in self.headers:
                length = int(self.headers['Content-Length'])
                self.wfile.write(self.rfile.read(length))
            else:
                data = []
                while True:
                    line = self.rfile.readline()
                    line = line.rstrip()
                    length = int(line, 16)
                    if length != 0:
                        data.append(self.rfile.read(length))
                    self.rfile.read(2)  # ignore CRLF
                    if length == 0:
                        break
                data = b''.join(data)
                self.wfile.write(data)
        else:
            self._404()


def server(ready, host, port):
    server_address = (host, port)
    httpd = HTTPServer(server_address, Handler)
    ready.set()
    httpd.serve_forever()


if __name__ == '__main__':
    ready = threading.Event()
    srv = threading.Thread(target=lambda: server(ready, '', 8090))
    srv.daemon = True
    srv.start()
    ready.wait()
    sys.exit(subprocess.call(
        sys.argv[1:],
        stdin=sys.stdin,
        stdout=sys.stdout,
        stderr=sys.stderr
    ))
