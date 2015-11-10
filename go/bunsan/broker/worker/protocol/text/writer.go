package text

import (
	"fmt"
	"io"

	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/golang/protobuf/proto"
)

type eventWriter struct {
	writer *rawEventWriter
}

func NewEventWriter(w io.Writer) protocol.EventWriter {
	return &eventWriter{NewRawEventWriter(w)}
}

func (w *eventWriter) WriteEvent(event protocol.Event) error {
	var revent RawEvent
	var err error
	switch ev := event.Kind.(type) {
	case *protocol.Event_Status:
		revent.Data, err = proto.Marshal(ev.Status)
		revent.Name = "status"
	case *protocol.Event_Result:
		revent.Data, err = proto.Marshal(ev.Result)
		revent.Name = "result"
	default:
		return fmt.Errorf("invalid event oneof")
	}
	if err != nil {
		return err
	}
	return w.writer.WriteRawEvent(revent)
}
