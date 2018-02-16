package text

import (
	"bytes"
	"io"
	"testing"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/stretchr/testify/assert"
)

func TestEventReaderEventWriter(t *testing.T) {
	testData := []struct {
		samples []protocol.Event
	}{
		{
			[]protocol.Event{}, // empty test
		},
		{
			[]protocol.Event{
				protocol.Event{
					&protocol.Event_Status{
						&broker.Status{
							Code:   123,
							Reason: "hello",
						},
					},
				},
				protocol.Event{
					&protocol.Event_Result{
						&broker.Result{
							Status: broker.Result_ERROR,
							Reason: "world",
						},
					},
				},
			},
		},
	}
	for _, tt := range testData {
		var buf bytes.Buffer
		writer := NewEventWriter(&buf)
		for _, sample := range tt.samples {
			err := writer.WriteEvent(sample)
			assert.NoError(t, err)
		}
		reader := NewEventReader(&buf)
		for _, sample := range tt.samples {
			event, err := reader.ReadEvent()
			assert.NoError(t, err)
			assert.Equal(t, sample, event)
		}
		_, err := reader.ReadEvent()
		assert.Equal(t, io.EOF, err)
	}
}

func TestEventWriterNilEvent(t *testing.T) {
	var buf bytes.Buffer
	writer := NewEventWriter(&buf)
	err := writer.WriteEvent(protocol.Event{})
	assert.EqualError(t, err, "invalid event oneof")
}
