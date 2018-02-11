package problem

import (
    "fmt"
    "os"
    "path"
    "strings"
    "sync"
)

var (
    importersMu sync.Mutex
    importers   = make(map[string]ImporterFactory)
)

const (
    formatLengthLimit = 1024
)

type Importer interface {
    Open(problemPath string) error
    Close() error

    Overview() *Problem

    BuildWorker(workerPath string) error
    BuildStatement(statementPath string) error
}

type ImporterFactory interface {
    New() (Importer, error)
}

func Register(name string, importer ImporterFactory) {
    importersMu.Lock()
    defer importersMu.Unlock()
    if importer == nil {
        panic("problem: Register importer is nil")
    }
    if _, dup := importers[name]; dup {
        panic("problem: Register called twice for importer " + name)
    }
    importers[name] = importer
}

func New(name string) (Importer, error) {
    importersMu.Lock()
    importerFactory, ok := importers[name]
    importersMu.Unlock()
    if !ok {
        return nil, fmt.Errorf("problem: unknown importer %q (forgotten import?)", name)
    }
    return importerFactory.New()
}

func Format(problemPath string) (string, error) {
    formatFile, err := os.Open(path.Join(problemPath, "format"))
    if err != nil {
        return "", fmt.Errorf("can't open format file: %v", err)
    }
    formatData := make([]byte, formatLengthLimit)
    formatLength, err := formatFile.Read(formatData)
    if formatLength == formatLengthLimit {
        return "", fmt.Errorf(
            "too big format name, expected less than %d", formatLengthLimit)
    }
    return strings.TrimSpace(string(formatData[:formatLength])), nil
}

// Parse format file and Open appropriate Importer
func Open(problemPath string) (Importer, error) {
    format, err := Format(problemPath)
    if err != nil {
        return nil, fmt.Errorf("can't open problem: %v", err)
    }
    importer, err := New(format)
    if err != nil {
        return nil, fmt.Errorf("can't open problem: %v", err)
    }
    err = importer.Open(problemPath)
    if err != nil {
        return nil, err
    }
    return importer, nil
}
