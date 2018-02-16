package service

import (
	"log"
	"sync"
	"time"

	"github.com/bunsanorg/broker/go/bunsan/broker"
	"github.com/streadway/amqp"
)

const (
	retryTimeout = time.Second
)

type taskReader func(connection *amqp.Connection) error

type RequestReader interface {
	Read(constraints broker.Constraints, jobs int) (<-chan Request, error)
	Close() error
}

type rabbitRequestReader struct {
	url       string
	aborter   chan struct{}
	abortOnce sync.Once
	tasks     chan Request
}

func NewRequestReader(url string) RequestReader {
	return &rabbitRequestReader{
		url:     url,
		aborter: make(chan struct{}, 1),
	}
}

func (r *rabbitRequestReader) maintainConnection(reader taskReader) error {
	working := true
	errCh := make(chan error, 1)
	for working {
		log.Printf("Dialing %q broker...", r.url)
		conn, err := amqp.Dial(r.url)
		if err != nil {
			log.Printf("Unable to connect to broker %q: %v", r.url, err)
			log.Printf("Retrying in %v", retryTimeout)
			select {
			case <-r.aborter:
				log.Printf("RequestReader is canceled")
				working = false
				close(r.tasks)
			case <-time.After(retryTimeout):
			}
			continue
		}
		log.Printf("Connected %q broker", r.url)
		go func() { errCh <- reader(conn) }()
		select {
		case <-r.aborter:
			log.Printf("RequestReader is canceled")
			working = false
			conn.Close()
			err = <-errCh
			if err != nil {
				log.Print(err)
			}
			close(r.tasks)
		case err = <-errCh:
			if err != nil {
				log.Print(err)
			} else {
				log.Print("Disconnected from %q broker", r.url)
			}
			conn.Close()
		}
	}
	return nil
}

func (r *rabbitRequestReader) Read(
	constraints broker.Constraints, jobs int) (<-chan Request, error) {

	r.tasks = make(chan Request, jobs)
	go r.maintainConnection(func(connection *amqp.Connection) error {
		var consumers sync.WaitGroup
		channel, err := connection.Channel()
		if err != nil {
			return err
		}
		defer channel.Close()
		err = channel.Qos(jobs, 0, false)
		if err != nil {
			return err
		}
		for _, resource := range constraints.Resource {
			queue, err := channel.QueueDeclare(
				resource, // name
				true,     // durable
				false,    // delete when unused
				false,    // exclusive
				false,    // no-wait
				nil,      // arguments
			)
			deliveries, err := channel.Consume(
				queue.Name, // queue
				"",         // consumer
				false,      // auto-ack
				false,      // exclusive
				false,      // no-local
				false,      // no-wait
				nil,        // args
			)
			if err != nil {
				return err
			}
			consumers.Add(1)
			go func() {
				for delivery := range deliveries {
					request, err := NewRequest(channel, &delivery)
					if err != nil {
						// note: error is reported by NewRequest()
						log.Printf("Unable to read request: %v", err)
						continue
					}
					log.Printf("Received request %q", delivery.CorrelationId)
					r.tasks <- request
				}
				consumers.Done()
			}()
		}
		consumers.Wait()
		return nil
	})
	return r.tasks, nil
}

func (r *rabbitRequestReader) Close() error {
	r.abortOnce.Do(func() {
		close(r.aborter)
	})
	return nil
}
