package text

import (
	"fmt"
	"io"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
	"github.com/golang/protobuf/proto"
)

type eventReader struct {
	reader *rawEventReader
}

func NewEventReader(r io.Reader) protocol.EventReader {
	return &eventReader{NewRawEventReader(r)}
}

func (r *eventReader) ReadEvent() (protocol.Event, error) {
	var status broker.Status
	var result broker.Result
	re, err := r.reader.ReadRawEvent()
	if err != nil {
		return protocol.Event{}, err
	}
	switch re.Name {
	case "status":
		err = proto.Unmarshal(re.Data, &status)
		if err != nil {
			return protocol.Event{}, err
		}
		return protocol.Event{&protocol.Event_Status{&status}}, nil
	case "result":
		err = proto.Unmarshal(re.Data, &result)
		if err != nil {
			return protocol.Event{}, err
		}
		return protocol.Event{&protocol.Event_Result{&result}}, nil
	default:
		return protocol.Event{}, fmt.Errorf("unknown event name %q", re.Name)
	}
}
