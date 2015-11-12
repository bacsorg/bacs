package driverutil

import (
	"errors"
	"io"
	"io/ioutil"
	"os/exec"
	"testing"

	"github.com/stretchr/testify/assert"
)

const (
	// TODO make platform-independent
	echoCommand    = "echo"
	echoArg        = "hello world"
	failCommand    = "false"
	stdoutData     = "hello world\n"
	unknownCommand = "definitely-not-existing-command-please-do-not-create-it"
)

var ErrTestParseFailed = errors.New("parse failed")

func TestStdoutRunner(t *testing.T) {
	runner := NewStdoutRunner()
	cmd := exec.Command(echoCommand, echoArg)
	err := runner.Run(cmd, func(stdout io.Reader) error {
		data, err := ioutil.ReadAll(stdout)
		assert.NoError(t, err)
		assert.Equal(t, stdoutData, string(data))
		return nil
	})
	assert.NoError(t, err)
}

func TestStdoutRunnerUnknownCommand(t *testing.T) {
	runner := NewStdoutRunner()
	cmd := exec.Command(unknownCommand)
	err := runner.Run(cmd, func(stdout io.Reader) error {
		assert.Fail(t, "Should not be called")
		return nil
	})
	execErr, ok := err.(*exec.Error)
	if assert.True(t, ok) {
		assert.Equal(t, exec.ErrNotFound, execErr.Err)
	}
}

func TestStdoutRunnerExitError(t *testing.T) {
	runner := NewStdoutRunner()
	cmd := exec.Command(failCommand)
	err := runner.Run(cmd, func(stdout io.Reader) error {
		data, err := ioutil.ReadAll(stdout)
		assert.NoError(t, err)
		assert.Equal(t, "", string(data))
		return nil
	})
	exitErr, ok := err.(*exec.ExitError)
	if assert.True(t, ok) {
		assert.True(t, exitErr.Exited())
		assert.False(t, exitErr.Success())
	}
}

func TestStdoutRunnerExitAndParserError(t *testing.T) {
	runner := NewStdoutRunner()
	cmd := exec.Command(failCommand)
	err := runner.Run(cmd, func(stdout io.Reader) error {
		data, err := ioutil.ReadAll(stdout)
		assert.NoError(t, err)
		assert.Equal(t, "", string(data))
		return ErrTestParseFailed
	})
	runnerErr, ok := err.(*RunnerError)
	if assert.True(t, ok) {
		assert.Equal(t, ErrTestParseFailed, runnerErr.ParserErr)
		exitErr, ok := runnerErr.ExecErr.(*exec.ExitError)
		if assert.True(t, ok) {
			assert.True(t, exitErr.Exited())
			assert.False(t, exitErr.Success())
		}
	}
}
