package worker

import (
	"errors"
	"fmt"
	"testing"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/service/mock"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/driver"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/driver/mock"
	"github.com/bunsanorg/pm/go/bunsan/pm/mock"
	"github.com/golang/mock/gomock"
	"github.com/stretchr/testify/assert"
)

const (
	DriverName        = "test-proxy-driver"
	WorkerDirectory   = "/some/worker/directory"
	PackageName       = "bunsan/pm/package/name"
	RequestData       = "request data"
	ErrDriverString   = "driver error"
	PanicDriverString = "driver panic"
)

var (
	ErrDriver      = errors.New(ErrDriverString)
	ErrPanicDriver = errors.New(PanicDriverString)
)

type ProxyDriver struct {
	impl driver.Driver
}

func (d *ProxyDriver) Run(task driver.Task) (broker.Result, error) {
	return d.impl.Run(task)
}

func (d *ProxyDriver) Set(driver driver.Driver) {
	if d.impl != nil {
		panic("ProxyDriver is set twice")
	}
	d.impl = driver
}

func (d *ProxyDriver) Clear() {
	if d.impl == nil {
		panic("ProxyDriver is cleared twice")
	}
	d.impl = nil
}

var proxyDriver ProxyDriver

func init() {
	driver.Register(DriverName, &proxyDriver)
}

type WorkerFixture struct {
	task       broker.Task
	status1    broker.Status
	status2    broker.Status
	status3    broker.Status
	result     broker.Result
	repository *mock_pm.MockRepository
	driver     *mock_driver.MockDriver
	request    *mock_service.MockRequest
	worker     Worker
}

func NewWorkerFixture(ctrl *gomock.Controller) *WorkerFixture {
	f := &WorkerFixture{
		task: broker.Task{
			Package: PackageName,
			Worker:  DriverName,
			Data:    []byte(RequestData),
		},
		status1: broker.Status{
			Code:   1,
			Reason: "status 1 reason",
			Data:   []byte("status 1 data"),
		},
		status2: broker.Status{
			Code:   2,
			Reason: "status 2 reason",
			Data:   []byte("status 2 data"),
		},
		status3: broker.Status{
			Code:   3,
			Reason: "status 3 reason",
			Data:   []byte("status 3 data"),
		},
		result: broker.Result{
			Status: broker.Result_OK,
			Reason: "reason",
			Data:   []byte("data"),
			Log:    []byte("log"),
		},
		repository: mock_pm.NewMockRepository(ctrl),
		driver:     mock_driver.NewMockDriver(ctrl),
		request:    mock_service.NewMockRequest(ctrl),
	}
	f.worker = NewWorker(f.repository, WorkerDirectory)
	f.request.EXPECT().Task().AnyTimes().Return(&f.task)
	proxyDriver.Set(f.driver)
	return f
}

func (f *WorkerFixture) Close() {
	proxyDriver.Clear()
}

func (f *WorkerFixture) DriverTaskMatcher() *DriverTaskMatcher {
	return &DriverTaskMatcher{&f.task}
}

type DriverTaskMatcher struct {
	BrokerTask *broker.Task
}

func (m *DriverTaskMatcher) Matches(x interface{}) bool {
	if task, ok := x.(driver.Task); ok {
		if task.BrokerTask != m.BrokerTask {
			return false
		}
		if task.WorkingDirectory != WorkerDirectory {
			return false
		}
		if task.StatusWriter == nil {
			return false
		}
		return true
	}
	return false
}

func (m *DriverTaskMatcher) String() string {
	return fmt.Sprintf("%v %q [non-nil channel]",
		m.BrokerTask, WorkerDirectory)
}

func TestWorkerDoOK(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerFixture(ctrl)
	defer f.Close()

	gomock.InOrder(
		f.repository.EXPECT().Extract(PackageName, WorkerDirectory),
		f.driver.EXPECT().Run(f.DriverTaskMatcher()).Return(f.result, nil).
			Do(func(t driver.Task) {
			t.StatusWriter <- f.status1
			t.StatusWriter <- f.status2
			t.StatusWriter <- f.status3
		}),
		f.request.EXPECT().WriteStatus(f.status1),
		f.request.EXPECT().WriteStatus(f.status2),
		f.request.EXPECT().WriteStatus(f.status3),
		f.request.EXPECT().WriteResult(f.result),
		f.request.EXPECT().Ack(),
	)

	err := f.worker.Do(f.request)
	assert.NoError(t, err)
}

func TestWorkerDoErr(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerFixture(ctrl)
	defer f.Close()

	gomock.InOrder(
		f.repository.EXPECT().Extract(PackageName, WorkerDirectory),
		f.driver.EXPECT().Run(f.DriverTaskMatcher()).
			Return(broker.Result{}, ErrDriver).Do(func(t driver.Task) {
			t.StatusWriter <- f.status1
			t.StatusWriter <- f.status2
			t.StatusWriter <- f.status3
		}),
		f.request.EXPECT().WriteStatus(f.status1),
		f.request.EXPECT().WriteStatus(f.status2),
		f.request.EXPECT().WriteStatus(f.status3),
		f.request.EXPECT().WriteError(ErrDriver),
		f.request.EXPECT().Nack(),
	)

	err := f.worker.Do(f.request)
	assert.EqualError(t, err, ErrDriverString)
}

func TestWorkerDoPanic(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewWorkerFixture(ctrl)
	defer f.Close()

	gomock.InOrder(
		f.repository.EXPECT().Extract(PackageName, WorkerDirectory),
		f.driver.EXPECT().Run(f.DriverTaskMatcher()).Do(func(t driver.Task) {
			t.StatusWriter <- f.status1
			t.StatusWriter <- f.status2
			t.StatusWriter <- f.status3
			panic(PanicDriverString)
		}),
		f.request.EXPECT().WriteStatus(f.status1),
		f.request.EXPECT().WriteStatus(f.status2),
		f.request.EXPECT().WriteStatus(f.status3),
		f.request.EXPECT().WriteError(ErrPanicDriver),
		f.request.EXPECT().Nack(),
	)

	err := f.worker.Do(f.request)
	assert.EqualError(t, err, PanicDriverString)
}
