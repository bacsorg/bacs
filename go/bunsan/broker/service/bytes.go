package service

import (
	"github.com/streadway/amqp"
)

type BytesWriter interface {
	WriteBytes(data []byte) error
}

type rabbitBytesWriter struct {
	Channel       *amqp.Channel
	Destination   string
	DeliveryMode  uint8
	CorrelationId string
}

func (w *rabbitBytesWriter) WriteBytes(data []byte) error {
	return w.Channel.Publish(
		"",            // exchange
		w.Destination, // routing key
		false,         // mandatory
		false,         // immediate
		amqp.Publishing{
			Body:          data,
			DeliveryMode:  w.DeliveryMode,
			CorrelationId: w.CorrelationId,
		})
}
