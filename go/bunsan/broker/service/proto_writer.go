package service

import (
	"github.com/golang/protobuf/proto"
)

type ProtoWriter interface {
	WriteProto(message proto.Message) error
}

type bytesProtoWriter struct {
	bytesWriter BytesWriter
}

func (w *bytesProtoWriter) WriteProto(message proto.Message) error {
	data, err := proto.Marshal(message)
	if err != nil {
		return err
	}
	return w.bytesWriter.WriteBytes(data)
}
