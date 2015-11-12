package driverutil

//go:generate bunsan-mockgen -gofile=$GOFILE

import (
	"io"
	"log"
	"os/exec"
)

type StdoutParser func(stdout io.Reader) error

type StdoutRunner interface {
	Run(cmd *exec.Cmd, p StdoutParser) error
}

func NewStdoutRunner() StdoutRunner {
	return &stdoutRunner{}
}

type stdoutRunner struct {
	cmd *exec.Cmd
}

func (r *stdoutRunner) Run(cmd *exec.Cmd, p StdoutParser) (err error) {
	stdout, err := r.cmd.StdoutPipe()
	if err != nil {
		return err
	}
	if err := r.cmd.Start(); err != nil {
		return err
	}
	defer func() {
		stdout.Close()
		werr := r.cmd.Wait()
		if err == nil {
			err = werr
		} else if werr != nil {
			log.Printf("Unhandled error: %v, during another error: %v",
				werr, err)
		}
	}()
	return p(stdout)
}
