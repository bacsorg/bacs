package driver

import (
	"bytes"
	"io"
	"io/ioutil"
	"os/exec"
	"path"
	"testing"

	"github.com/bacsorg/bacs/go/bunsan/broker"
	"github.com/bacsorg/bacs/go/bunsan/broker/worker/driver/driverutil"
	"github.com/bacsorg/bacs/go/bunsan/broker/worker/driver/driverutil/mock"
	"github.com/bacsorg/bacs/go/bunsan/broker/worker/protocol"
	"github.com/bacsorg/bacs/go/bunsan/broker/worker/protocol/text"
	"github.com/golang/mock/gomock"
	"github.com/stretchr/testify/assert"
)

type ProxyTextDriver struct {
	driver Driver
}

func (d ProxyTextDriver) Run(task Task) (broker.Result, error) {
	return d.driver.Run(task)
}

var proxyTextDriver ProxyTextDriver

const (
	proxyTextDriverName = "proxy-text-driver"
	executableName      = "text-driver-executable"
	workingDirectory    = "text-driver-working-directory"
	inputData           = "text-driver-input-data"
	resultData          = "text-driver-result-data"
	logData             = "text-driver-log-data"
)

func init() {
	Register(proxyTextDriverName, &proxyTextDriver)
}

type TextDriverFixture struct {
	statusWriter chan broker.Status
	runner       *mock_driverutil.MockStdoutRunner
	task         Task
	statuses     []broker.Status
	result       broker.Result
}

func NewTextDriverFixture(ctrl *gomock.Controller) *TextDriverFixture {
	statusWriter := make(chan broker.Status, 3)
	f := &TextDriverFixture{
		statusWriter: statusWriter,
		runner:       mock_driverutil.NewMockStdoutRunner(ctrl),
		task: Task{
			BrokerTask: &broker.Task{
				Worker:  proxyTextDriverName,
				Package: "",
				Data:    []byte(inputData),
			},
			WorkingDirectory: workingDirectory,
			StatusWriter:     statusWriter,
		},
		statuses: []broker.Status{
			broker.Status{
				Code:   1,
				Reason: "reason 1",
			},
			broker.Status{
				Code:   2,
				Reason: "reason 2",
			},
			broker.Status{
				Code:   3,
				Reason: "reason 3",
			},
		},
		result: broker.Result{
			Status: broker.Result_OK,
			Reason: "",
			Data:   []byte(resultData),
		},
	}
	proxyTextDriver.driver = &TextDriver{
		Executable: executableName,
		Runner:     f.runner,
	}
	return f
}

func (f *TextDriverFixture) CheckCmd(t *testing.T, cmd *exec.Cmd) {
	exePath := path.Join(workingDirectory, executableName)
	assert.Equal(t, exePath, cmd.Path)
	assert.Equal(t, workingDirectory, cmd.Dir)
	assert.Equal(t, []string{exePath}, cmd.Args)
	stdinData, err := ioutil.ReadAll(cmd.Stdin)
	assert.NoError(t, err)
	assert.Equal(t, []byte(inputData), stdinData)
}

func (f *TextDriverFixture) CheckNoStatus(t *testing.T) {
	select {
	case status, closed := <-f.statusWriter:
		if closed {
			assert.Fail(t, "Closed status writer, but expected not to")
		} else {
			assert.Fail(t, "Unexpected status: %v", status)
		}
	default:
	}
}

func (f *TextDriverFixture) Close() {
	proxyTextDriver.driver = nil
}

func TestTextDriver(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewTextDriverFixture(ctrl)
	defer f.Close()

	f.runner.EXPECT().Run(gomock.Any(), gomock.Any()).
		Do(func(cmd *exec.Cmd, p driverutil.StdoutParser) {
		f.CheckCmd(t, cmd)
		_, err := io.WriteString(cmd.Stderr, logData)
		assert.NoError(t, err)
		var buf bytes.Buffer
		w := text.NewEventWriter(&buf)
		for _, status := range f.statuses {
			err = w.WriteEvent(protocol.Event{&protocol.Event_Status{&status}})
			assert.NoError(t, err)
		}
		err = w.WriteEvent(protocol.Event{&protocol.Event_Result{&f.result}})
		assert.NoError(t, err)
		err = p(&buf)
		assert.NoError(t, err)
	})

	result, err := Run(f.task)
	for _, expected := range f.statuses {
		actual := <-f.statusWriter
		assert.Equal(t, expected, actual)
	}
	f.CheckNoStatus(t)
	assert.NoError(t, err)
	assert.Equal(t, broker.Result_OK, result.Status)
	assert.Equal(t, "", result.Reason)
	assert.Equal(t, resultData, string(result.Data))
	assert.Equal(t, logData, string(result.Log))
}

func TestTextDriverExecutionError(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewTextDriverFixture(ctrl)
	defer f.Close()

	f.runner.EXPECT().Run(gomock.Any(), gomock.Any()).
		Return(&exec.ExitError{}).
		Do(func(cmd *exec.Cmd, p driverutil.StdoutParser) {
		var buf bytes.Buffer
		_, err := io.WriteString(cmd.Stderr, logData)
		assert.NoError(t, err)
		err = p(&buf)
		assert.NoError(t, err)
	})

	result, err := Run(f.task)
	assert.NoError(t, err)
	f.CheckNoStatus(t)
	assert.Equal(t, broker.Result_EXECUTION_ERROR, result.Status)
	assert.Equal(t, "<nil>", result.Reason)
	assert.Equal(t, "", string(result.Data))
	assert.Equal(t, logData, string(result.Log))
}

func TestTextDriverExecutionAndProtocolError(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()

	f := NewTextDriverFixture(ctrl)
	defer f.Close()

	f.runner.EXPECT().Run(gomock.Any(), gomock.Any()).
		Return(&exec.ExitError{}).
		Do(func(cmd *exec.Cmd, p driverutil.StdoutParser) {
		var buf bytes.Buffer
		_, err := buf.WriteString("hello d29ybGQ=\n")
		assert.NoError(t, err)
		_, err = io.WriteString(cmd.Stderr, logData)
		assert.NoError(t, err)
		err = p(&buf)
		assert.EqualError(t, err, "unknown event name \"hello\"")
	})

	result, err := Run(f.task)
	assert.NoError(t, err)
	f.CheckNoStatus(t)
	assert.Equal(t, broker.Result_EXECUTION_ERROR, result.Status)
	assert.Equal(t, "<nil>", result.Reason)
	assert.Equal(t, "", string(result.Data))
	assert.Equal(t, logData, string(result.Log))
}
