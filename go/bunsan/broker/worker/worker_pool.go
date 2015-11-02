package worker

import (
	"log"
	"runtime"
	"sync"

	"github.com/bunsanorg/broker/go/bunsan/broker/service"
)

type WorkerPool interface {
	// Add new worker, can be called any time.
	Add(worker Worker) error
	// Process requests from channel.
	DoAll(requests <-chan service.Request) error
	// Finish last request gracefully, do not process new requests.
	Cancel() error
	// Abort everything now.
	Abort() error
}

type workerPool struct {
	workersWaitGroup sync.WaitGroup
	workers          []Worker
	requests         chan service.Request
	canceler         chan struct{}
}

func NewWorkerPool() WorkerPool {
	return &workerPool{
		workers:  make([]Worker, 0, runtime.NumCPU()),
		requests: make(chan service.Request),
		canceler: make(chan struct{}),
	}
}

func (p *workerPool) run(worker Worker) {
	for request := range p.requests {
		err := worker.Do(request)
		if err != nil {
			log.Print(err)
		}
	}
	p.workersWaitGroup.Done()
}

func (p *workerPool) Add(worker Worker) error {
	p.workers = append(p.workers, worker)
	p.workersWaitGroup.Add(1)
	go p.run(worker)
	return nil
}

func (p *workerPool) DoAll(requests <-chan service.Request) error {
	for run := true; run; {
		select {
		case request, ok := <-requests:
			if ok {
				select {
				case p.requests <- request:
					// just continue
				case <-p.canceler:
					run = false
				}
			} else {
				run = false
			}
		case <-p.canceler:
			run = false
		}
	}
	close(p.requests)
	p.workersWaitGroup.Wait()
	return nil
}

func (p *workerPool) Cancel() error {
	close(p.canceler)
	return nil
}

func (p *workerPool) Abort() error {
	err := p.Cancel()
	if err != nil {
		return err
	}
	for _, worker := range p.workers {
		werr := worker.Abort()
		if werr != nil {
			// FIXME returns some error
			err = werr
		}
	}
	return err
}
