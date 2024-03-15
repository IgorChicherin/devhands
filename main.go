package main

import (
	"crypto/rand"
	"crypto/sha256"
	"fmt"
	"syscall"
	"time"
)

func main() {
	loadTimeMs := 500 * time.Millisecond
	targetUsage := 500 * time.Millisecond
	data := make([]byte, 1024*1024)

	_, err := rand.Read(data)

	if err != nil {
		fmt.Printf("Error: %s", err)
	}

	usage := syscall.Rusage{}
	syscall.Getrusage(syscall.RUSAGE_SELF, &usage)

	startUsage := usage.Utime.Usec + usage.Stime.Usec
	fmt.Printf("CPU time start: %d\n", startUsage)

	var delta int64

	for {
		h := sha256.New()
		h.Write(data)
		data = h.Sum(nil)

		syscall.Getrusage(syscall.RUSAGE_SELF, &usage)
		currentUsage := usage.Utime.Usec + usage.Stime.Usec
		delta = currentUsage - startUsage

		fmt.Printf("CPU delta: %d\n", delta)
		fmt.Printf("Work time: %d\n", targetUsage.Microseconds())

		if delta > targetUsage.Microseconds() {
			break
		}
	}

	time.Sleep(loadTimeMs)
}
