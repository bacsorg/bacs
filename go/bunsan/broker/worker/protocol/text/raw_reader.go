package text

import (
	"bufio"
	"encoding/base64"
	"fmt"
	"io"
	"strings"
)

type rawEventReader struct {
	reader *bufio.Reader
}

func NewRawEventReader(r io.Reader) *rawEventReader {
	return &rawEventReader{bufio.NewReader(r)}
}

func (r *rawEventReader) ReadRawEvent() (RawEvent, error) {
	line, err := r.reader.ReadString('\n')
	if err != nil {
		return RawEvent{}, err
	}
	line = strings.Trim(line, " ")
	tokens := strings.Split(line, " ")
	if len(tokens) != 2 {
		return RawEvent{}, fmt.Errorf("invalid number of tokens: "+
			"expected 2, got %d", len(tokens))
	}
	data, err := base64.StdEncoding.DecodeString(tokens[1])
	if err != nil {
		return RawEvent{}, err
	}
	return RawEvent{
		Name: tokens[0],
		Data: data,
	}, nil
}
