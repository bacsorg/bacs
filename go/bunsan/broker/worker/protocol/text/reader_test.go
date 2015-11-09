package text

import (
	"io"
	"strings"
	"testing"

	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/stretchr/testify/assert"
)

func TestReader(t *testing.T) {
	testData := []struct {
		data   string
		events []protocol.Event
	}{
		{
			data: "hello d29ybGQ=\n",
			events: []protocol.Event{
				protocol.Event{"hello", []byte("world")},
			},
		},
		{
			data: "first cGFydA==\n" +
				"other ZXZlbnQ=\n" +
				"third bWVzc2FnZQ==\n",
			events: []protocol.Event{
				protocol.Event{"first", []byte("part")},
				protocol.Event{"other", []byte("event")},
				protocol.Event{"third", []byte("message")},
			},
		},
	}
	for _, tt := range testData {
		reader := NewEventReader(strings.NewReader(tt.data))
		for _, event := range tt.events {
			e, err := reader.ReadEvent()
			assert.NoError(t, err)
			assert.Equal(t, event, e)
		}
		_, err := reader.ReadEvent()
		assert.Equal(t, io.EOF, err)
	}
}

func TestReaderErrNumberOfTokens(t *testing.T) {
	reader := NewEventReader(strings.NewReader("\n"))
	_, err := reader.ReadEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")

	reader = NewEventReader(strings.NewReader("invalid\n"))
	_, err = reader.ReadEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")

	reader = NewEventReader(strings.NewReader("invalid token sequence\n"))
	_, err = reader.ReadEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 3")
}

func TestReaderErrBase64(t *testing.T) {
	reader := NewEventReader(strings.NewReader("some =invalid\n"))
	_, err := reader.ReadEvent()
	assert.EqualError(t, err, "illegal base64 data at input byte 0")
}

func TestReaderErrRecover(t *testing.T) {
	reader := NewEventReader(strings.NewReader(
		"invalid\nsome =invalid\nhello d29ybGQ=\n"))
	_, err := reader.ReadEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")
	_, err = reader.ReadEvent()
	assert.EqualError(t, err, "illegal base64 data at input byte 0")
	event, err := reader.ReadEvent()
	assert.NoError(t, err)
	assert.Equal(t, protocol.Event{"hello", []byte("world")}, event)
	_, err = reader.ReadEvent()
	assert.Equal(t, io.EOF, err)
}
