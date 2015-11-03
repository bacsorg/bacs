package service

//go:generate bunsan-mockgen -gofile=$GOFILE

import (
	"fmt"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/bunsanorg/broker/go/bunsan/broker/rabbit"
	"github.com/golang/protobuf/proto"
	"github.com/streadway/amqp"
)

type Request interface {
	StatusWriter
	ResultWriter
	ErrorWriter
	Task() *broker.Task
	Ack() error
	Nack() error
}

type rabbitRequest struct {
	task         rabbit.RabbitTask
	statusWriter StatusWriter
	resultWriter ResultWriter
	errorWriter  ErrorWriter
	delivery     *amqp.Delivery
}

// Create new request object, return and send error if it is not possible.
func NewRequest(
	channel *amqp.Channel, delivery *amqp.Delivery) (Request, error) {

	r := &rabbitRequest{
		delivery: delivery,
		errorWriter: NewErrorWriter(
			channel, delivery.ReplyTo, delivery.CorrelationId),
	}
	err := proto.Unmarshal(delivery.Body, &r.task)
	if err != nil {
		r.errorWriter.WriteError(err)
		return nil, err
	}
	if delivery.CorrelationId != r.task.Identifier {
		err = fmt.Errorf(
			"invalid task: correlation id %q does not match task id %q",
			delivery.CorrelationId, r.task.Identifier)
		r.errorWriter.WriteError(err)
		return nil, err
	}
	r.statusWriter = NewStatusWriter(
		channel, r.task.StatusQueue, r.task.Identifier)
	r.resultWriter = NewResultWriter(
		channel, r.task.ResultQueue, r.task.Identifier)
	return r, nil
}

func (r *rabbitRequest) Task() *broker.Task {
	return r.task.Task
}

func (r *rabbitRequest) WriteStatus(status broker.Status) error {
	return r.statusWriter.WriteStatus(status)
}

func (r *rabbitRequest) WriteResult(result broker.Result) error {
	return r.resultWriter.WriteResult(result)
}

func (r *rabbitRequest) WriteError(err error) error {
	return r.errorWriter.WriteError(err)
}

func (r *rabbitRequest) Ack() error {
	return r.delivery.Ack(false)
}

func (r *rabbitRequest) Nack() error {
	return r.delivery.Nack(false, false)
}
