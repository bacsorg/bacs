package pm

import (
	"encoding/json"
	"fmt"
	"reflect"
)

type SnapshotEntry map[string]string
type Snapshot map[string]SnapshotEntry

func (s SnapshotEntry) Equal(o SnapshotEntry) bool {
	return reflect.DeepEqual(s, o)
}

func (s SnapshotEntry) Clone() SnapshotEntry {
	clone := make(SnapshotEntry)
	for k, v := range s {
		clone[k] = v
	}
	return clone
}

func (s Snapshot) Equal(o Snapshot) bool {
	return reflect.DeepEqual(s, o)
}

func (s Snapshot) Clone() Snapshot {
	clone := make(Snapshot)
	for k, v := range s {
		clone[k] = v
	}
	return clone
}

func (s Snapshot) Marshal() ([]byte, error) {
	data, err := json.MarshalIndent(s, "", "    ")
	if err != nil {
		return nil, fmt.Errorf("unable to marshal snapshot: %v", err)
	}
	return data, nil
}

func (s *Snapshot) Unmarshal(data []byte) error {
	err := json.Unmarshal(data, s)
	if err != nil {
		return fmt.Errorf("unable to unmarshal snapshot: %v", err)
	}
	return nil
}
