package problem

import (
    "io/ioutil"
    "os"
    "path"
    "testing"

    "github.com/stretchr/testify/assert"
    "github.com/stretchr/testify/require"
)

type testImporter struct {
    problemPath string
}

type testImporterFactory struct{}

func (f *testImporterFactory) New() (Importer, error) {
    return &testImporter{}, nil
}

func (imp *testImporter) Open(problemPath string) error {
    imp.problemPath = problemPath
    return nil
}
func (imp *testImporter) Close() error                     { return nil }
func (imp *testImporter) Overview() *Problem               { return nil }
func (imp *testImporter) BuildWorker(path string) error    { return nil }
func (imp *testImporter) BuildStatement(path string) error { return nil }

func init() {
    Register("test", &testImporterFactory{})
}

func TestRegisterExisting(t *testing.T) {
    assert.Panics(t, func() { Register("test", &testImporterFactory{}) })
}

func TestNew(t *testing.T) {
    importer, err := New("test")
    if assert.NoError(t, err) {
        assert.IsType(t, &testImporter{}, importer)
    }
}

func TestFormat(t *testing.T) {
    tmpdir, err := ioutil.TempDir("", "")
    require.NoError(t, err)
    defer os.RemoveAll(tmpdir)

    formatFile, err := os.Create(path.Join(tmpdir, "format"))
    require.NoError(t, err)
    formatFile.WriteString("my_problem_format")
    require.NoError(t, formatFile.Close())

    format, err := Format(tmpdir)
    if assert.NoError(t, err) {
        assert.Equal(t, "my_problem_format", format)
    }
}

func TestOpen(t *testing.T) {
    tmpdir, err := ioutil.TempDir("", "")
    require.NoError(t, err)
    defer os.RemoveAll(tmpdir)

    formatFile, err := os.Create(path.Join(tmpdir, "format"))
    require.NoError(t, err)
    formatFile.WriteString("test")
    require.NoError(t, formatFile.Close())

    importer, err := Open(tmpdir)
    if assert.NoError(t, err) {
        if assert.IsType(t, &testImporter{}, importer) {
            imp := importer.(*testImporter)
            assert.Equal(t, tmpdir, imp.problemPath)
        }
    }
}
