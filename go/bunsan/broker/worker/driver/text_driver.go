package driver

import (
	"bytes"
	"io"
	"log"
	"os/exec"
	"path"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/driver/driverutil"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol/text"
)

// FIXME should not be placed here
func init() {
	Register("bacs_single", TextDriver{
		Executable: path.Join("bin", "bacs_system_single_worker"),
		Runner:     driverutil.NewStdoutRunner(),
	})
}

type TextDriver struct {
	Executable string
	Runner     driverutil.StdoutRunner
}

func (d TextDriver) Run(task Task) (broker.Result, error) {
	var wlog bytes.Buffer
	var result broker.Result
	executable := path.Join(task.WorkingDirectory, d.Executable)
	cmd := exec.Command(executable)
	cmd.Dir = task.WorkingDirectory
	cmd.Stdin = bytes.NewReader(task.BrokerTask.Data)
	cmd.Stderr = &wlog
	err := d.Runner.Run(cmd, func(stdout io.ReadCloser) error {
		reader := text.NewEventReader(stdout)
		event, err := reader.ReadEvent()
		for ; err == nil; event, err = reader.ReadEvent() {
			switch ev := event.Kind.(type) {
			case *protocol.Event_Status:
				task.StatusWriter <- *ev.Status
			case *protocol.Event_Result:
				result = *ev.Result
			default:
				log.Fatal("Unknown event type (oneof extended?): %v", event)
			}
		}
		if err != io.EOF {
			return err
		}
		return nil
	})
	if err != nil {
		if exitErr, ok := err.(*exec.ExitError); ok {
			err = nil
			result.Status = broker.Result_EXECUTION_ERROR
			result.Reason = exitErr.Error()
		}
	}
	result.Log = wlog.Bytes()
	return result, err
}
