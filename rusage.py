import resource
import hashlib

WORKLOAD_TIME_MS = 1000

if __name__ == "__main__":
    workload_ms = WORKLOAD_TIME_MS * 1_000
    start_usage = resource.getrusage(resource.RUSAGE_SELF)
    start_usage = (start_usage.ru_utime + start_usage.ru_stime) * 1_000
    delta = 0

    with open("/dev/random", 'rb') as f:
        data = f.read(1024*1024)

    while True:
        data = hashlib.md5(data).digest()
        current_usage = resource.getrusage(resource.RUSAGE_SELF)
        current_usage = (
            current_usage.ru_utime + current_usage.ru_stime
        ) * 1_000

        delta += current_usage - start_usage

        print(f"Cpu delta {delta} ms")
        print(f"Workload {workload_ms} ms")

        if (workload_ms - delta < 0):
            break
