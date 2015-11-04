package worker

import (
	"testing"

	"github.com/bunsanorg/broker/go/bunsan/broker/service"
	"github.com/bunsanorg/broker/go/bunsan/broker/service/mock"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/mock"
	"github.com/golang/mock/gomock"
	"github.com/stretchr/testify/assert"
)

type WorkerPoolFixture struct {
	req1, req2, req3     service.Request
	requests             chan service.Request
	wp                   WorkerPool
	w1, w2               *mock_worker.MockWorker
	w1CanStart, w1CanEnd chan struct{}
	w2CanStart, w2CanEnd chan struct{}
	done                 chan struct{}
}

func NewWorkerPoolFixture(ctrl *gomock.Controller) *WorkerPoolFixture {
	return &WorkerPoolFixture{
		req1:       mock_service.NewMockRequest(ctrl),
		req2:       mock_service.NewMockRequest(ctrl),
		req3:       mock_service.NewMockRequest(ctrl),
		requests:   make(chan service.Request, 3),
		wp:         NewWorkerPool(),
		w1:         mock_worker.NewMockWorker(ctrl),
		w2:         mock_worker.NewMockWorker(ctrl),
		w1CanStart: make(chan struct{}),
		w1CanEnd:   make(chan struct{}),
		w2CanStart: make(chan struct{}),
		w2CanEnd:   make(chan struct{}),
		done:       make(chan struct{}),
	}
}

func (f *WorkerPoolFixture) Start(t *testing.T) {
	go func() {
		t.Logf("req1 = %v\nreq2 = %v\nreq3 = %v", f.req1, f.req2, f.req3)
		f.wp.DoAll(f.requests)
		close(f.done)
	}()
}

func (f *WorkerPoolFixture) Finish() {
	close(f.requests)
	<-f.done
}

func TestWorkerPoolAdd(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerPoolFixture(ctrl)
	f.Start(t)

	gomock.InOrder(
		f.w1.EXPECT().Do(f.req1).Do(func(r interface{}) {
			t.Log("w1 begin 1")
			close(f.w2CanStart)
			t.Log("w1 mid 1")
			<-f.w1CanEnd
			t.Log("w1 end 1")
		}),
		f.w2.EXPECT().Do(f.req2).Do(func(r interface{}) {
			t.Log("w2 begin")
			close(f.w1CanEnd)
			t.Log("w2 mid")
			<-f.w2CanEnd
			t.Log("w2 end")
		}),
		f.w1.EXPECT().Do(f.req3).Do(func(r interface{}) {
			t.Log("w1 begin 2")
			close(f.w2CanEnd)
			t.Log("w1 end 2")
		}),
	)

	f.wp.Add(f.w1)
	f.requests <- f.req1
	f.requests <- f.req2
	f.requests <- f.req3
	<-f.w2CanStart
	f.wp.Add(f.w2)

	f.Finish()
}

func TestWorkerPoolCancel(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerPoolFixture(ctrl)
	f.Start(t)

	gomock.InOrder(
		f.w1.EXPECT().Do(f.req1).Do(func(r interface{}) {
			t.Log("w1 run")
		}),
		f.w1.EXPECT().Do(f.req2).Do(func(r interface{}) {
			t.Log("w1 begin 2")
			f.wp.Cancel()
			close(f.w2CanStart)
			t.Log("w1 end 2")
		}),
	)

	t.Log("begin")
	f.wp.Add(f.w1)
	f.requests <- f.req1
	f.requests <- f.req2
	f.requests <- f.req3
	<-f.w2CanStart
	assert.Panics(t, func() { f.wp.Add(f.w2) })
	t.Log("end")

	<-f.done
	assert.Equal(t, f.req3, <-f.requests) // was not consumed
}

func TestWorkerPoolCancelWithoutAdd(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerPoolFixture(ctrl)
	f.Start(t)

	f.wp.Cancel()
	<-f.done
}

func TestWorkerPoolAbort(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerPoolFixture(ctrl)
	f.Start(t)

	w1Do := f.w1.EXPECT().Do(f.req1).Do(func(r interface{}) {
		t.Log("w1 begin")
		close(f.w2CanStart)
		t.Log("w1 mid")
		<-f.w1CanEnd
		t.Log("w1 end")
	})
	f.w1.EXPECT().Abort().After(w1Do).Do(func() {
		close(f.w1CanEnd)
	})
	f.w2.EXPECT().Abort().After(w1Do)

	f.wp.Add(f.w1)
	f.requests <- f.req1
	<-f.w2CanStart
	f.wp.Add(f.w2)
	f.wp.Abort()

	<-f.done
}
