package pm

import "time"

//go:generate bunsan-mockgen -gofile=$GOFILE

type Repository interface {
	Create(sourcePath string, strip bool) error
	CreateRecursively(rootPath string, strip bool) error
	Extract(pkg, destination string) error
	Install(pkg, destination string) error
	ForceUpdate(pkg, destination string) error
	Update(pkg, destination string, lifetime time.Time) error
	NeedUpdate(pkg, destination string, lifetime time.Time) (bool, error)
	CleanCache() error
	Close() error
}
