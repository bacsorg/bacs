package pm

// #cgo LDFLAGS: -lbunsan_pm_compatibility
// #include <bunsan/pm/compatibility/repository.h>
// #include <stdlib.h>
import "C"

import (
	"time"
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
	cConfig := C.CString(config)
	defer C.free(unsafe.Pointer(cConfig))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRepo := C.bunsan_pm_repository_new(cConfig, (*C.char)(cErr), errSize)
	if cRepo == nil {
		return nil, &cError{C.GoString((*C.char)(cErr))}
	}
	return &cRepository{cRepo}, nil
}

func (r *cRepository) Close() error {
	C.bunsan_pm_repository_free(r.repo)
	r.repo = nil
	return nil
}

func (r *cRepository) Create(path string, strip bool) error {
	cPath := C.CString(path)
	defer C.free(unsafe.Pointer(cPath))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRet := C.bunsan_pm_repository_create(
		r.repo, cPath, C.bool(strip), (*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) CreateRecursively(rootPath string, strip bool) error {
	cRootPath := C.CString(rootPath)
	defer C.free(unsafe.Pointer(cRootPath))
	cErr := C.malloc(errSize)
	defer C.free(unsafe.Pointer(cErr))
	cRet := C.bunsan_pm_repository_create_recursively(r.repo, cRootPath,
		C.bool(strip), (*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) Extract(pkg, destination string) error {
	cPkg := C.CString(pkg)
	defer C.free(unsafe.Pointer(cPkg))
	cDst := C.CString(destination)
	defer C.free(unsafe.Pointer(cDst))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRet := C.bunsan_pm_repository_extract(
		r.repo, cPkg, cDst, (*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) Install(pkg, destination string) error {
	cPkg := C.CString(pkg)
	defer C.free(unsafe.Pointer(cPkg))
	cDst := C.CString(destination)
	defer C.free(unsafe.Pointer(cDst))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRet := C.bunsan_pm_repository_install(r.repo, cPkg, cDst,
		(*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) ForceUpdate(pkg, destination string) error {
	cPkg := C.CString(pkg)
	defer C.free(unsafe.Pointer(cPkg))
	cDst := C.CString(destination)
	defer C.free(unsafe.Pointer(cDst))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRet := C.bunsan_pm_repository_force_update(r.repo, cPkg, cDst,
		(*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) Update(pkg, destination string, lifetime time.Time) error {
	cPkg := C.CString(pkg)
	defer C.free(unsafe.Pointer(cPkg))
	cDst := C.CString(destination)
	defer C.free(unsafe.Pointer(cDst))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	cRet := C.bunsan_pm_repository_update(r.repo, cPkg, cDst,
		C.time_t(lifetime.Unix()), (*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}

func (r *cRepository) NeedUpdate(pkg, destination string, lifetime time.Time) (
	bool, error) {

	cPkg := C.CString(pkg)
	defer C.free(unsafe.Pointer(cPkg))
	cDst := C.CString(destination)
	defer C.free(unsafe.Pointer(cDst))
	cErr := C.malloc(errSize)
	defer C.free(cErr)
	var cNeed C._Bool
	cRet := C.bunsan_pm_repository_need_update(r.repo, cPkg, cDst,
		C.time_t(lifetime.Unix()), &cNeed,
		(*C.char)(cErr), errSize)
	if cRet != 0 {
		return bool(cNeed), &cError{C.GoString((*C.char)(cErr))}
	}
	return bool(cNeed), nil
}

func (r *cRepository) CleanCache() error {
	cErr := C.malloc(errSize)
	defer C.free(unsafe.Pointer(cErr))
	cRet := C.bunsan_pm_repository_clean_cache(r.repo, (*C.char)(cErr), errSize)
	if cRet != 0 {
		return &cError{C.GoString((*C.char)(cErr))}
	}
	return nil
}
