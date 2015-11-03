package worker

import (
	"testing"

	"github.com/bunsanorg/broker/go/bunsan/broker/service"
	"github.com/bunsanorg/broker/go/bunsan/broker/service/mock"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/mock"
	"github.com/golang/mock/gomock"
)

func TestWorkerPoolAdd(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	done := make(chan struct{})
	requests := make(chan service.Request, 3)

	request1 := mock_service.NewMockRequest(ctrl)
	request2 := mock_service.NewMockRequest(ctrl)
	request3 := mock_service.NewMockRequest(ctrl)

	wp := NewWorkerPool()
	{
		go func() {
			wp.DoAll(requests)
			close(done)
		}()
	}

	w1CanEnd := make(chan struct{})
	w2CanEnd := make(chan struct{})
	w1 := mock_worker.NewMockWorker(ctrl)
	w2 := mock_worker.NewMockWorker(ctrl)
	gomock.InOrder(
		w1.EXPECT().Do(gomock.Eq(request1)).Do(func(r interface{}) {
			t.Log("w1 begin 1")
			<-w1CanEnd
			t.Log("w1 end 1")
		}),
		w1.EXPECT().Do(gomock.Eq(request3)).Do(func(r interface{}) {
			t.Log("w1 begin 2")
			close(w2CanEnd)
			t.Log("w1 end 2")
		}),
	)
	w2.EXPECT().Do(gomock.Eq(request2)).Do(func(r interface{}) {
		t.Log("w2 begin")
		close(w1CanEnd)
		t.Log("w2 mid")
		<-w2CanEnd
		t.Log("w2 end")
	})

	wp.Add(w1)
	requests <- request1
	requests <- request2
	requests <- request3
	wp.Add(w2)

	close(requests)
	<-done
}
