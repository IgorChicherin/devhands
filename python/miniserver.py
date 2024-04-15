#!/usr/bin/python3

import hashlib
import resource
import time
from flask import Flask


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


app = Flask("miniserver")


@app.route("/python/hello-world")
def hello_world():
    return "<h1>Hello, World!</h1>"


@app.route("/python/cpu-workload/<int:cpu_usage>/<int:sleep>")
def cpu_workload(cpu_usage, sleep):
    do_cpu_workload(cpu_usage_ms=cpu_usage, sleep_time_ms=sleep)
    return "<h1>Done</h1>"
