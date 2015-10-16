package pm

import "reflect"

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
