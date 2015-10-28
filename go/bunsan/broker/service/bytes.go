package service

import (
	"github.com/assembla/cony"
	"github.com/streadway/amqp"
)

type BytesWriter interface {
	WriteBytes(data []byte) error
}

type rabbitBytesWriter struct {
	publisher     *cony.Publisher
	deliveryMode  uint8
	correlationId string
}

func NewNonPersistentBytesWriter(
	publisher *cony.Publisher, correlationId string) BytesWriter {

	return &rabbitBytesWriter{
		publisher:     publisher,
		deliveryMode:  1,
		correlationId: correlationId,
	}
}

func NewPersistentBytesWriter(
	publisher *cony.Publisher, correlationId string) BytesWriter {

	return &rabbitBytesWriter{
		publisher:     publisher,
		deliveryMode:  2,
		correlationId: correlationId,
	}
}

func (w *rabbitBytesWriter) WriteBytes(data []byte) error {
	return w.publisher.Publish(amqp.Publishing{
		Body:          data,
		DeliveryMode:  w.deliveryMode,
		CorrelationId: w.correlationId,
	})
}
