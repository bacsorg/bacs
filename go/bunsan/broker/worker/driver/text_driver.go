package driver

import (
	"bufio"
	"bytes"
	"encoding/base64"
	"fmt"
	"log"
	"os/exec"
	"path"
	"strings"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/golang/protobuf/proto"
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
	var status broker.Status
	var wlog bytes.Buffer
	executable := path.Join(task.WorkingDirectory, d.executable)
	cmd := exec.Command(executable)
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
		close(task.StatusWriter)
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
	scanner := bufio.NewScanner(stdout)
	for scanner.Scan() {
		tokens := strings.Split(scanner.Text(), " ")
		if len(tokens) != 2 {
			return broker.Result{}, TextProtocolError{
				fmt.Errorf("invalid number of tokens: "+
					"expected 2, got %d", len(tokens))}
		}
		data, err := base64.StdEncoding.DecodeString(tokens[1])
		if err != nil {
			return broker.Result{}, TextProtocolError{err}
		}
		switch tokens[0] {
		case "status":
			err = proto.Unmarshal(data, &status)
			if err != nil {
				return broker.Result{}, TextProtocolError{err}
			}
			task.StatusWriter <- status
		case "result":
			err = proto.Unmarshal(data, &result)
			if err != nil {
				return broker.Result{}, TextProtocolError{err}
			}
		}
	}
	if err := scanner.Err(); err != nil {
		return broker.Result{}, TextProtocolError{err}
	}
	return
}
