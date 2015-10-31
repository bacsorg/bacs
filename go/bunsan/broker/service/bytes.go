package service

import (
	"github.com/streadway/amqp"
)

type BytesWriter interface {
	WriteBytes(data []byte) error
}

type rabbitBytesWriter struct {
	channel       *amqp.Channel
	destination   string
	deliveryMode  uint8
	correlationId string
}

func NewNonPersistentBytesWriter(
	channel *amqp.Channel,
	destination, correlationId string) BytesWriter {

	return &rabbitBytesWriter{
		channel:       channel,
		destination:   destination,
		deliveryMode:  1,
		correlationId: correlationId,
	}
}

func NewPersistentBytesWriter(
	channel *amqp.Channel,
	destination, correlationId string) BytesWriter {

	return &rabbitBytesWriter{
		channel:       channel,
		destination:   destination,
		deliveryMode:  2,
		correlationId: correlationId,
	}
}

func (w *rabbitBytesWriter) WriteBytes(data []byte) error {
	return w.channel.Publish(
		"",            // exchange
		w.destination, // routing key
		false,         // mandatory
		false,         // immediate
		amqp.Publishing{
			Body:          data,
			DeliveryMode:  w.deliveryMode,
			CorrelationId: w.correlationId,
		})
}
