package local

import (
	"io/ioutil"
	"os"
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestSystem(t *testing.T) {
	tmpdir, err := ioutil.TempDir("", "")
	require.NoError(t, err)
	defer os.RemoveAll(tmpdir)

	sys := NewSystem(SystemConfig{BuildTempDir: tmpdir})
	{
		tmpFile1, err := sys.SmallTempFile()
		if assert.NoError(t, err) {
			defer os.Remove(tmpFile1.Name())
			defer tmpFile1.Close()
			tmpFile2, err := sys.SmallTempFile()
			if assert.NoError(t, err) {
				defer os.Remove(tmpFile2.Name())
				defer tmpFile2.Close()
				assert.NotEqual(t, tmpFile1.Name(), tmpFile2.Name())
			}
		}
	}
	{
		tmpDir1, err := sys.TempDirForBuild()
		if assert.NoError(t, err) {
			assert.True(t, strings.HasPrefix(tmpDir1, tmpdir))
			tmpDir2, err := sys.TempDirForBuild()
			if assert.NoError(t, err) {
				assert.True(t, strings.HasPrefix(tmpDir2, tmpdir))
				assert.NotEqual(t, tmpDir1, tmpDir2)
			}
		}
	}
}
