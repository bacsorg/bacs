package text

import (
	"bufio"
	"encoding/base64"
	"io"
)

type rawEventWriter struct {
	writer *bufio.Writer
}

func NewRawEventWriter(r io.Writer) *rawEventWriter {
	return &rawEventWriter{bufio.NewWriter(r)}
}

func (w *rawEventWriter) WriteRawEvent(event RawEvent) error {
	_, err := w.writer.WriteString(event.Name)
	if err != nil {
		return err
	}
	err = w.writer.WriteByte(' ')
	if err != nil {
		return err
	}
	_, err = w.writer.WriteString(
		base64.StdEncoding.EncodeToString(event.Data))
	if err != nil {
		return err
	}
	err = w.writer.WriteByte('\n')
	if err != nil {
		return err
	}
	return w.writer.Flush()
}
