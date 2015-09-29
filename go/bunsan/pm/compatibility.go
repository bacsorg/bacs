package pm

// #cgo LDFLAGS: -lbunsan_pm_compatibility
// #include <bunsan/pm/compatibility/repository.h>
// #include <stdlib.h>
import "C"

import (
    "runtime"
    "unsafe"
)

const (
    errSize = 1024 * 1024
)

type cRepository struct {
    repo C.bunsan_pm_repository
}

type cError struct {
    message string
}

func (e *cError) Error() string {
    return e.message
}

func NewRepository(config string) (Repository, error) {
    c_config := C.CString(config)
    defer C.free(unsafe.Pointer(c_config))
    c_error := C.malloc(errSize)
    defer C.free(c_error)
    c_repo := C.bunsan_pm_repository_new(c_config, (*C.char)(c_error), errSize)
    if c_repo == nil {
        return nil, &cError{C.GoString((*C.char)(c_error))}
    }
    repo := &cRepository{c_repo}
    runtime.SetFinalizer(repo, func(r *cRepository) {
        r.Destroy()
    })
    return repo, nil
}

func (r *cRepository) Destroy() {
    C.bunsan_pm_repository_free(r.repo)
}

func (r *cRepository) Create(path string, strip bool) error {
    c_path := C.CString(path)
    defer C.free(unsafe.Pointer(c_path))
    c_error := C.malloc(errSize)
    defer C.free(c_error)
    c_ret := C.bunsan_pm_repository_create(
        r.repo, c_path, C.bool(strip), (*C.char)(c_error), errSize)
    if c_ret != 0 {
        return &cError{C.GoString((*C.char)(c_error))}
    }
    return nil
}

func (r *cRepository) CleanCache() error {
    c_error := C.malloc(errSize)
    defer C.free(unsafe.Pointer(c_error))
    c_ret := C.bunsan_pm_repository_clean_cache(r.repo, (*C.char)(c_error), errSize)
    if c_ret != 0 {
        return &cError{C.GoString((*C.char)(c_error))}
    }
    return nil
}

func (r *cRepository) Extract(pkg, destination string) error {
    c_pkg := C.CString(pkg)
    defer C.free(unsafe.Pointer(c_pkg))
    c_dst := C.CString(destination)
    defer C.free(unsafe.Pointer(c_dst))
    c_error := C.malloc(errSize)
    defer C.free(c_error)
    c_ret := C.bunsan_pm_repository_extract(
        r.repo, c_pkg, c_dst, (*C.char)(c_error), errSize)
    if c_ret != 0 {
        return &cError{C.GoString((*C.char)(c_error))}
    }
    return nil
}
