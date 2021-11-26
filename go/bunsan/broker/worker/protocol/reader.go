package protocol

type EventReader interface {
	ReadEvent() (Event, error)
}
