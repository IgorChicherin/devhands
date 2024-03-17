package main

import (
	"crypto/rand"
	"crypto/sha256"
	"fmt"
	"log"
	"net/http"
	"syscall"
	"time"
)

func GetCpuTime(usage syscall.Rusage) int64 {
	return usage.Utime.Nano() + usage.Stime.Nano()
}

func DoCpuWorkload(sleepTime, cpuUsageMs time.Duration) {
	data := make([]byte, 1024*1024)

	_, err := rand.Read(data)

	if err != nil {
		fmt.Printf("Error: %s", err)
	}

	usage := syscall.Rusage{}
	err = syscall.Getrusage(syscall.RUSAGE_SELF, &usage)

	if err != nil {
		fmt.Printf("Error: %s", err)
	}

	startUsageNs := GetCpuTime(usage)
	fmt.Printf("CPU time start: %d\n", startUsageNs)

	var delta int64

	for {
		err = syscall.Getrusage(syscall.RUSAGE_SELF, &usage)
		if err != nil {
			fmt.Printf("Error: %s", err)
		}
		currentUsageNs := GetCpuTime(usage)
		delta = currentUsageNs - startUsageNs

		if delta > cpuUsageMs.Nanoseconds() {
			break
		}

		h := sha256.New()
		h.Write(data)
		data = h.Sum(nil)

	}
	time.Sleep(sleepTime)
}

func HelloWorldHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello World!")
}

func DoCpuWorkloadHandler(w http.ResponseWriter, req *http.Request) {
	cpuLoad := req.PathValue("cpuLoadMs")
	l, err := time.ParseDuration(fmt.Sprintf("%sms", cpuLoad))

	if err != nil {
		fmt.Printf("Error: %s", err)
	}

	sleepTime := req.PathValue("sleepTime")
	st, err := time.ParseDuration(fmt.Sprintf("%sms", sleepTime))

	if err != nil {
		fmt.Printf("Error: %s", err)
	}

	fmt.Printf("CPU load: %s\n", l)
	fmt.Printf("Sleep time: %s\n", st)

	DoCpuWorkload(l, st)
}

func main() {
	http.HandleFunc("/hello-world", HelloWorldHandler)
	http.HandleFunc("/cpu-workload/{cpuLoadMs}/{sleepTime}", DoCpuWorkloadHandler)

	err := http.ListenAndServe(":3002", nil)

	if err != nil {
		log.Fatalf("Error: %s", err)
	}
}
