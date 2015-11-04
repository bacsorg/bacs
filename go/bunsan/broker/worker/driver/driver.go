package driver

//go:generate bunsan-mockgen -gofile=$GOFILE

import (
	"fmt"
	"sync"

	"github.com/bunsanorg/broker/go/bunsan/broker"
)

var (
	driversMu sync.Mutex
	drivers   = make(map[string]Driver)
)

type Task struct {
	BrokerTask       *broker.Task
	WorkingDirectory string
	StatusWriter     chan<- broker.Status
}

func (task Task) ReadDriverName() (string, error) {
	if task.BrokerTask.Worker == "" {
		return "", fmt.Errorf("empty driver name")
	}
	return task.BrokerTask.Worker, nil
}

type Driver interface {
	Run(task Task) (broker.Result, error)
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

func Run(task Task) (broker.Result, error) {
	driverName, err := task.ReadDriverName()
	if err != nil {
		return broker.Result{}, err
	}
	driversMu.Lock()
	driver, ok := drivers[driverName]
	driversMu.Unlock()
	if !ok {
		return broker.Result{}, fmt.Errorf(
			"worker: unknown driver %q", driverName)
	}
	return driver.Run(task)
}
