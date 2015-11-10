package driver

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"os/exec"
	"path"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol/text"
)

// FIXME should not be placed here
func init() {
	Register("bacs_single", TextDriver{
		executable: path.Join("bin", "bacs_system_single_worker"),
	})
}

type TextDriver struct {
	executable string
}

type TextProtocolError struct {
	Err error
}

func (e TextProtocolError) Error() string {
	return fmt.Sprintf("worker text protocol error: %v", e.Err)
}

func (d TextDriver) Run(task Task) (result broker.Result, err error) {
	var wlog bytes.Buffer
	executable := path.Join(task.WorkingDirectory, d.executable)
	cmd := exec.Command(executable)
	cmd.Dir = task.WorkingDirectory
	cmd.Stdin = bytes.NewReader(task.BrokerTask.Data)
	cmd.Stderr = &wlog
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return broker.Result{}, err
	}
	if err := cmd.Start(); err != nil {
		return broker.Result{}, nil
	}
	defer func() {
		stdout.Close()
		werr := cmd.Wait()
		if err != nil {
			log.Printf("Unhandled error: %v, during another error: %v",
				werr, err)
		} else {
			err = werr
		}
		result.Log = wlog.Bytes()
	}()
	reader := text.NewEventReader(stdout)
	event, err := reader.ReadEvent()
	for ; err == nil; event, err = reader.ReadEvent() {
		switch ev := event.Kind.(type) {
		case *protocol.Event_Status:
			task.StatusWriter <- *ev.Status
		case *protocol.Event_Result:
			result = *ev.Result
		default:
			log.Fatal("Unknown event type: %v", event)
		}
	}
	if err == io.EOF {
		err = nil
	}
	return
}
