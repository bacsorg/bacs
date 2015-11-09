package protocol

type EventWriter interface {
	WriteEvent(event Event) error
}
