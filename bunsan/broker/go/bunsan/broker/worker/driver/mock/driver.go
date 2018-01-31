// Automatically generated by MockGen. DO NOT EDIT!
// Source: github.com/bunsanorg/broker/go/bunsan/broker/worker/driver (interfaces: Driver)

package mock_driver

import (
	broker "github.com/bunsanorg/broker/go/bunsan/broker"
	driver "github.com/bunsanorg/broker/go/bunsan/broker/worker/driver"
	gomock "github.com/golang/mock/gomock"
)

// Mock of Driver interface
type MockDriver struct {
	ctrl     *gomock.Controller
	recorder *_MockDriverRecorder
}

// Recorder for MockDriver (not exported)
type _MockDriverRecorder struct {
	mock *MockDriver
}

func NewMockDriver(ctrl *gomock.Controller) *MockDriver {
	mock := &MockDriver{ctrl: ctrl}
	mock.recorder = &_MockDriverRecorder{mock}
	return mock
}

func (_m *MockDriver) EXPECT() *_MockDriverRecorder {
	return _m.recorder
}

func (_m *MockDriver) Run(_param0 driver.Task) (broker.Result, error) {
	ret := _m.ctrl.Call(_m, "Run", _param0)
	ret0, _ := ret[0].(broker.Result)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

func (_mr *_MockDriverRecorder) Run(arg0 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCall(_mr.mock, "Run", arg0)
}
