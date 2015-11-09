package text

import (
	"bufio"
	"encoding/base64"
	"fmt"
	"io"
	"strings"

	"github.com/bunsanorg/broker/go/bunsan/broker/worker/protocol"
)

type eventReader struct {
	reader *bufio.Reader
}

func NewEventReader(r io.Reader) protocol.EventReader {
	return &eventReader{bufio.NewReader(r)}
}

func (r *eventReader) ReadEvent() (protocol.Event, error) {
	line, err := r.reader.ReadString('\n')
	if err != nil {
		return protocol.Event{}, err
	}
	line = strings.Trim(line, " ")
	tokens := strings.Split(line, " ")
	if len(tokens) != 2 {
		return protocol.Event{}, fmt.Errorf("invalid number of tokens: "+
			"expected 2, got %d", len(tokens))
	}
	data, err := base64.StdEncoding.DecodeString(tokens[1])
	if err != nil {
		return protocol.Event{}, err
	}
	return protocol.Event{
		Name: tokens[0],
		Data: data,
	}, nil
}
