package path

import (
    "strings"
)

const (
    Separator  = "/"
    ParentPath = ".."
    ThisPath   = "."
)

func Split(path string) []string {
    elements := make([]string, 0, 10)
    for _, elem := range strings.Split(path, Separator) {
        if elem != "" {
            elements = append(elements, elem)
        }
    }
    return elements
}

func Join(elem ...string) string {
    path := make([]string, 0, 10)
    for _, e := range elem {
        path = append(path, Split(e)...)
    }
    return strings.Join(path, Separator)
}

func Absolute(path, root string) string {
    pathElements := Split(path)
    if len(pathElements) > 0 &&
        (pathElements[0] == ThisPath || pathElements[0] == ParentPath) {

        rootElements := Split(root)
        pathElements = append(rootElements, pathElements...)
    }
    absPathElems := make([]string, 0, len(pathElements))
    for _, elem := range pathElements {
        switch elem {
        case "":
            // drop
        case ThisPath:
            // drop
        case ParentPath:
            if len(absPathElems) > 0 {
                absPathElems = absPathElems[:len(absPathElems)-1]
            }
        default:
            absPathElems = append(absPathElems, elem)
        }
    }
    // it is fine to use strings.Join() here since we skipped all ""
    return strings.Join(absPathElems, Separator)
}
