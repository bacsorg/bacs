package worker

import (
	"fmt"
	"sync"

	"github.com/bunsanorg/broker/go/bunsan/broker"
)

var (
	driversMu sync.Mutex
	drivers   = make(map[string]Driver)
)

type Driver interface {
	Run(task *broker.Task,
		statusWriter chan<- broker.Status) (broker.Result, error)
}

func Register(name string, driver Driver) {
	driversMu.Lock()
	defer driversMu.Unlock()
	if driver == nil {
		panic("worker: Register driver is nil")
	}
	if _, dup := drivers[name]; dup {
		panic("worker: Register called twice for driver " + name)
	}
	drivers[name] = driver
}

func run(task *broker.Task,
	statusWriter chan<- broker.Status) (broker.Result, error) {

	driversMu.Lock()
	driver, ok := drivers[task.Worker]
	driversMu.Unlock()
	if !ok {
		return broker.Result{}, fmt.Errorf(
			"worker: unknown driver %q", task.Worker)
	}
	return driver.Run(task, statusWriter)
}
