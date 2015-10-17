package local

import (
    "io/ioutil"
    "os"
)

type System interface {
    // Empty directory for possibly large files.
    TempDirForBuild() (string, error)

    // Empty file.
    SmallTempFile() (*os.File, error)
}

type SystemConfig struct {
    BuildTempDir string
}

type system struct {
    config SystemConfig
}

func NewSystem(config SystemConfig) System {
    return &system{config}
}

func (s *system) TempDirForBuild() (string, error) {
    return ioutil.TempDir(s.config.BuildTempDir, "")
}

func (s *system) SmallTempFile() (*os.File, error) {
    return ioutil.TempFile("", "")
}
