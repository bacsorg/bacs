package worker

//go:generate bunsan-mockgen -gofile=$GOFILE

import (
	"errors"
	"fmt"
	"log"

	"github.com/bacsorg/bacs/go/bunsan/broker"
	"github.com/bacsorg/bacs/go/bunsan/broker/service"
	"github.com/bacsorg/bacs/go/bunsan/broker/worker/driver"
	"github.com/bacsorg/bacs/go/bunsan/pm"
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

func (w *worker) do(request service.Request) (broker.Result, error) {
	err := w.repository.Extract(request.Task().Package, w.dir)
	if err != nil {
		return broker.Result{
			Status: broker.Result_PACKAGE_ERROR,
			Reason: err.Error(),
		}, nil
	}
	statuses := make(chan broker.Status, numberOfBufferedStatuses)
	done := make(chan struct{})
	go func() {
		for status := range statuses {
			err := request.WriteStatus(status)
			if err != nil {
				log.Printf("Unable to write status: %v", err)
			}
		}
		close(done)
	}()
	defer func() {
		close(statuses)
		<-done
	}()
	return driver.Run(driver.Task{
		BrokerTask:       request.Task(),
		WorkingDirectory: w.dir,
		StatusWriter:     statuses,
	})
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
	result, err := w.do(request)
	if err != nil {
		return
	}
	err = request.WriteResult(result)
	return
}

func (w *worker) Abort() error {
	return ErrCantAbort
}
