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
    ret := make([]string, 0, 10)
    for _, elem := range strings.Split(path, Separator) {
        if elem != "" {
            ret = append(ret, elem)
        }
    }
    return ret
}

func Join(elem ...string) string {
    ret := make([]string, 0, 10)
    for _, e := range elem {
        for _, p := range Split(e) {
            ret = append(ret, p)
        }
    }
    return strings.Join(ret, Separator)
}
