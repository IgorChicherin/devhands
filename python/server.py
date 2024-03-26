import hashlib
import resource
import time
from http.server import HTTPServer, BaseHTTPRequestHandler


class HelloWorldHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/hello":
            self.hello_world()
        elif self.path.startswith("/cpu-workload"):
            self.cpu_workload()

    def hello_world(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write('<html><head><meta charset="utf-8">'.encode())
        self.wfile.write("<title>Hello.</title></head>".encode())
        self.wfile.write("<body><h1>Hello world!</h1></body></html>".encode())

    def cpu_workload(self):
        params = self.path.strip("/cpu-workload").split("/", maxsplit=2)
        sleep_time, cpu_time = params
        sleep_time, cpu_time = int(sleep_time), int(cpu_time)
        do_cpu_workload(sleep_time_ms=sleep_time, cpu_usage_ms=cpu_time)

        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write('<html><head><meta charset="utf-8">'.encode())
        self.wfile.write("<title>Hello.</title></head>".encode())
        self.wfile.write(f"<body><h1>{params}</h1></body></html>".encode())


def do_cpu_workload(sleep_time_ms: int, cpu_usage_ms: int):
    sleep_time = sleep_time_ms / 1000
    cpu_usage = cpu_usage_ms / 1000

    start_usage = resource.getrusage(resource.RUSAGE_SELF)
    start_usage = start_usage.ru_utime + start_usage.ru_stime

    with open("/dev/random", 'rb') as f:
        data = f.read(1024 * 1024)

    while True:
        data = hashlib.sha256(data).digest()
        current_usage = resource.getrusage(resource.RUSAGE_SELF)
        current_usage = current_usage.ru_utime + current_usage.ru_stime

        delta = current_usage - start_usage

        if delta > cpu_usage:
            break

    time.sleep(sleep_time)


def run_server(server_class=HTTPServer, handler_class=BaseHTTPRequestHandler):
    server_address = ("", 3003)
    httpd = server_class(server_address, handler_class)

    try:
        httpd.serve_forever()

    except KeyboardInterrupt:
        httpd.server_close()


if __name__ == "__main__":
    run_server(handler_class=HelloWorldHandler)
