package text

import (
	"bytes"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestRawEventWriter(t *testing.T) {
	testData := []struct {
		events []RawEvent
		data   string
	}{
		{
			events: []RawEvent{
				RawEvent{"hello", []byte("world")},
			},
			data: "hello d29ybGQ=\n",
		},
		{
			events: []RawEvent{
				RawEvent{"first", []byte("part")},
				RawEvent{"other", []byte("event")},
				RawEvent{"third", []byte("message")},
			},
			data: "first cGFydA==\n" +
				"other ZXZlbnQ=\n" +
				"third bWVzc2FnZQ==\n",
		},
	}
	for _, tt := range testData {
		var buf bytes.Buffer
		writer := NewRawEventWriter(&buf)
		for _, event := range tt.events {
			err := writer.WriteRawEvent(event)
			assert.NoError(t, err)
		}
		assert.Equal(t, tt.data, string(buf.Bytes()))
	}
}
