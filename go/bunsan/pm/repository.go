package pm

type Repository interface {
    Create(path string, strip bool) error
    CleanCache() error
    Extract(pkg, destination string) error
}
