package worker

import (
	"errors"
	"fmt"
	"log"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/service"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/driver"
	"github.com/bunsanorg/pm/go/bunsan/pm"
)

var ErrAborted = errors.New("worker was aborted")
var ErrCantAbort = errors.New("worker can't be aborted: not supported")

const (
	numberOfBufferedStatuses = 100
)

type Worker interface {
	Do(request service.Request) error
	Abort() error
}

type worker struct {
	repository pm.Repository
	dir        string
}

func NewWorker(repository pm.Repository, dir string) Worker {
	return &worker{repository, dir}
}

func (w *worker) do(request service.Request) error {
	err := w.repository.Extract(request.Task().Package, w.dir)
	if err != nil {
		return err
	}
	statuses := make(chan broker.Status, numberOfBufferedStatuses)
	done := make(chan struct{})
	defer func() { <-done }()
	go func() {
		for status := range statuses {
			err := request.WriteStatus(status)
			if err != nil {
				log.Printf("Unable to write status: %v", err)
			}
		}
		close(done)
	}()
	result, err := driver.Run(request.Task(), statuses)
	if err != nil {
		return err
	}
	request.WriteResult(result)
	return nil
}

func (w *worker) Do(request service.Request) (err error) {
	defer func() {
		if r := recover(); r != nil {
			switch t := r.(type) {
			case error:
				err = t
			case string:
				err = errors.New(t)
			default:
				err = fmt.Errorf("%v", t)
			}
		}
		if err != nil {
			log.Printf("Unable to finish request: %v", err)
			request.WriteError(err)
			request.Nack()
		} else {
			request.Ack()
		}
	}()
	err = w.do(request)
	return
}

func (w *worker) Abort() error {
	return ErrCantAbort
}
