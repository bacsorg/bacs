package pm

//go:generate bunsan-mockgen -gofile=$GOFILE

type Repository interface {
	Create(path string, strip bool) error
	CleanCache() error
	Extract(pkg, destination string) error
	Close() error
}
