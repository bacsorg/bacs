package driverutil

//go:generate bunsan-mockgen -gofile=$GOFILE

import (
	"fmt"
	"io"
	"os/exec"
)

type StdoutParser func(stdout io.Reader) error

type StdoutRunner interface {
	Run(cmd *exec.Cmd, p StdoutParser) error
}

type RunnerError struct {
	ExecErr   error
	ParserErr error
}

func (e *RunnerError) Error() string {
	return fmt.Sprintf("parser error: %v; exec error: %v",
		e.ParserErr, e.ExecErr)
}

func NewStdoutRunner() StdoutRunner {
	return &stdoutRunner{}
}

type stdoutRunner struct{}

func (r *stdoutRunner) Run(cmd *exec.Cmd, p StdoutParser) (err error) {
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return err
	}
	if err := cmd.Start(); err != nil {
		return err
	}
	defer func() {
		stdout.Close()
		werr := cmd.Wait()
		if err == nil {
			err = werr
		} else if werr != nil {
			err = &RunnerError{
				ExecErr:   werr,
				ParserErr: err,
			}
		}
	}()
	return p(stdout)
}
