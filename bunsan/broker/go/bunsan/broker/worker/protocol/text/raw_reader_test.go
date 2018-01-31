package text

import (
	"io"
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestRawEventReader(t *testing.T) {
	testData := []struct {
		data   string
		events []RawEvent
	}{
		{
			data: "hello d29ybGQ=\n",
			events: []RawEvent{
				RawEvent{"hello", []byte("world")},
			},
		},
		{
			data: "first cGFydA==\n" +
				"other ZXZlbnQ=\n" +
				"third bWVzc2FnZQ==\n",
			events: []RawEvent{
				RawEvent{"first", []byte("part")},
				RawEvent{"other", []byte("event")},
				RawEvent{"third", []byte("message")},
			},
		},
	}
	for _, tt := range testData {
		reader := NewRawEventReader(strings.NewReader(tt.data))
		for _, event := range tt.events {
			e, err := reader.ReadRawEvent()
			assert.NoError(t, err)
			assert.Equal(t, event, e)
		}
		_, err := reader.ReadRawEvent()
		assert.Equal(t, io.EOF, err)
	}
}

func TestRawEventReaderErrNumberOfTokens(t *testing.T) {
	reader := NewRawEventReader(strings.NewReader("\n"))
	_, err := reader.ReadRawEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")

	reader = NewRawEventReader(strings.NewReader("invalid\n"))
	_, err = reader.ReadRawEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")

	reader = NewRawEventReader(strings.NewReader("invalid token sequence\n"))
	_, err = reader.ReadRawEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 3")
}

func TestRawEventReaderErrBase64(t *testing.T) {
	reader := NewRawEventReader(strings.NewReader("some =invalid\n"))
	_, err := reader.ReadRawEvent()
	assert.EqualError(t, err, "illegal base64 data at input byte 0")
}

func TestRawEventReaderErrRecover(t *testing.T) {
	reader := NewRawEventReader(strings.NewReader(
		"invalid\nsome =invalid\nhello d29ybGQ=\n"))
	_, err := reader.ReadRawEvent()
	assert.EqualError(t, err, "invalid number of tokens: expected 2, got 1")
	_, err = reader.ReadRawEvent()
	assert.EqualError(t, err, "illegal base64 data at input byte 0")
	event, err := reader.ReadRawEvent()
	assert.NoError(t, err)
	assert.Equal(t, RawEvent{"hello", []byte("world")}, event)
	_, err = reader.ReadRawEvent()
	assert.Equal(t, io.EOF, err)
}
