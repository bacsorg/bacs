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
	// Finish all consumed requests, do not process new requests.
	Cancel() error
	// Abort everything now, drop consumed requests.
	Abort() error
}

type requestsChannelWaiter struct {
	requestsReady *sync.Cond
	requests      <-chan service.Request
}

func NewRequestsChannelWaiter() *requestsChannelWaiter {
	return &requestsChannelWaiter{
		requestsReady: sync.NewCond(&sync.Mutex{}),
		requests:      nil,
	}
}

func (r *requestsChannelWaiter) Get() <-chan service.Request {
	r.requestsReady.L.Lock()
	for r.requests == nil {
		r.requestsReady.Wait()
	}
	r.requestsReady.L.Unlock()
	return r.requests
}

func (r *requestsChannelWaiter) Set(requests <-chan service.Request) {
	r.requestsReady.L.Lock()
	if r.requests != nil {
		panic("\"requests\" is set twice")
	}
	r.requests = requests
	r.requestsReady.L.Unlock()
	r.requestsReady.Broadcast()
}

type workerPool struct {
	workersWaitGroup sync.WaitGroup
	workers          []Worker
	requests         *requestsChannelWaiter
	canceler         chan struct{}
	closeFirstWorker sync.Once
	firstWorker      chan struct{}
}

func NewWorkerPool() WorkerPool {
	return &workerPool{
		workers:     make([]Worker, 0, runtime.NumCPU()),
		requests:    NewRequestsChannelWaiter(),
		canceler:    make(chan struct{}),
		firstWorker: make(chan struct{}),
	}
}

func (p *workerPool) do(worker Worker, request service.Request) {
	err := worker.Do(request)
	if err != nil {
		log.Print(err)
	}
}

func (p *workerPool) run(worker Worker) {
	requests := p.requests.Get()
	for run := true; run; {
		select {
		case <-p.canceler:
			run = false
		default:
			select {
			case request, ok := <-requests:
				if !ok {
					run = false
					continue
				}
				p.do(worker, request)
			case <-p.canceler:
				run = false
			}
		}
	}
	p.workersWaitGroup.Done()
}

func (p *workerPool) Add(worker Worker) error {
	select {
	case <-p.canceler:
		panic("Called Add() after Cancel()")
	default:
	}
	p.workers = append(p.workers, worker)
	p.workersWaitGroup.Add(1)
	go p.run(worker)
	p.closeFirstWorker.Do(func() { close(p.firstWorker) })
	return nil
}

func (p *workerPool) DoAll(requests <-chan service.Request) error {
	p.requests.Set(requests)
	<-p.firstWorker
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
