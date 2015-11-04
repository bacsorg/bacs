package pm

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestSnapshotEntry(t *testing.T) {
	a := SnapshotEntry{
		"hello": "checksum1",
		"world": "checksum2",
	}
	b := a.Clone()
	assert.Equal(t, a, b)
	assert.True(t, &a != &b)
	assert.True(t, a.Equal(a))
	assert.True(t, a.Equal(b))
	assert.True(t, b.Equal(a))
	assert.True(t, b.Equal(b))
}

func TestSnapshot(t *testing.T) {
	a := Snapshot{
		"package1": SnapshotEntry{
			"a": "chksum1",
			"b": "chksum2",
		},
		"package2": SnapshotEntry{
			"c": "chksum3",
			"d": "chksum4",
		},
	}
	b := a.Clone()
	assert.True(t, &a != &b)
	assert.Equal(t, a, b)
	assert.True(t, a.Equal(a))
	assert.True(t, a.Equal(b))
	assert.True(t, b.Equal(a))
	assert.True(t, b.Equal(b))

	data, err := a.Marshal()
	assert.NoError(t, err)
	assert.Regexp(t, `(?s).*"package1":.*"package2":.*`, string(data))
	var c Snapshot
	err = c.Unmarshal(data)
	assert.NoError(t, err)
	assert.True(t, c.Equal(a))
	assert.Equal(t, a, c)
}
