package service

import (
	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/streadway/amqp"
)

type StatusWriter interface {
	WriteStatus(status broker.Status) error
}

type ResultWriter interface {
	WriteResult(result broker.Result) error
}

type ErrorWriter interface {
	WriteError(err error) error
}

type ResponseWriter interface {
	StatusWriter
	ResultWriter
	ErrorWriter
}

type rabbitResponseWriter struct {
	statusWriter, resultWriter ProtoWriter
	errorWriter                BytesWriter
}

func (w *rabbitResponseWriter) WriteStatus(status broker.Status) error {
	return w.statusWriter.WriteProto(&status)
}

func (w *rabbitResponseWriter) WriteResult(result broker.Result) error {
	return w.resultWriter.WriteProto(&result)
}

func (w *rabbitResponseWriter) WriteError(err error) error {
	return w.errorWriter.WriteBytes([]byte(err.Error()))
}

func NewResponseWriter(
	channel *amqp.Channel,
	statusQueue, resultQueue, errorQueue,
	correlationId string) ResponseWriter {

	return &rabbitResponseWriter{
		statusWriter: &bytesProtoWriter{
			NewNonPersistentBytesWriter(
				channel, statusQueue, correlationId),
		},
		resultWriter: &bytesProtoWriter{
			NewPersistentBytesWriter(
				channel, resultQueue, correlationId),
		},
		errorWriter: NewPersistentBytesWriter(
			channel, errorQueue, correlationId),
	}
}
