package service

import (
	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/rabbit"
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

type rabbitStatusWriter struct {
	correlationId string
	writer        ProtoWriter
}

func NewStatusWriter(
	channel *amqp.Channel, queue, correlationId string) StatusWriter {

	return &rabbitStatusWriter{
		correlationId: correlationId,
		writer: &bytesProtoWriter{
			&rabbitBytesWriter{
				Channel:       channel,
				DeliveryMode:  amqp.Transient,
				Destination:   queue,
				CorrelationId: correlationId,
			},
		},
	}
}

func (w *rabbitStatusWriter) WriteStatus(status broker.Status) error {
	return w.writer.WriteProto(&rabbit.RabbitStatus{
		Identifier: w.correlationId,
		Status:     &status,
	})
}

type rabbitResultWriter struct {
	correlationId string
	writer        ProtoWriter
}

func NewResultWriter(
	channel *amqp.Channel, queue, correlationId string) ResultWriter {

	return &rabbitResultWriter{
		correlationId: correlationId,
		writer: &bytesProtoWriter{
			&rabbitBytesWriter{
				Channel:       channel,
				DeliveryMode:  amqp.Persistent,
				Destination:   queue,
				CorrelationId: correlationId,
			},
		},
	}
}

func (w *rabbitResultWriter) WriteResult(result broker.Result) error {
	return w.writer.WriteProto(&rabbit.RabbitResult{
		Identifier: w.correlationId,
		Result:     &result,
	})
}

type rabbitErrorWriter struct {
	correlationId string
	writer        BytesWriter
}

func NewErrorWriter(
	channel *amqp.Channel, queue, correlationId string) ErrorWriter {

	return &rabbitErrorWriter{
		correlationId: correlationId,
		writer: &rabbitBytesWriter{
			Channel:       channel,
			DeliveryMode:  amqp.Persistent,
			Destination:   queue,
			CorrelationId: correlationId,
		},
	}
}

func (w *rabbitErrorWriter) WriteError(err error) error {
	return w.writer.WriteBytes([]byte(err.Error()))
}
