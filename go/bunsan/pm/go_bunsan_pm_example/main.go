package main

import (
    "flag"
    "github.com/bunsanorg/pm/go/bunsan/pm"
)

var config = flag.String("config", "pm.rc", "Repository configuration file")

func main() {
    flag.Parse()
    repo, err := pm.NewRepository(*config)
    if err != nil {
        panic(err)
    }
    defer repo.Close()
    err = repo.Extract("hello", "world")
    if err != nil {
        panic(err)
    }
}
