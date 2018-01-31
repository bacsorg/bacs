package main

import (
    "flag"
    "log"

    "github.com/bunsanorg/pm/go/bunsan/pm"
)

var config = flag.String("config", "pm.rc", "Repository configuration file")

func main() {
    flag.Parse()
    repo, err := pm.NewRepository(*config)
    if err != nil {
        log.Fatal(err)
    }
    defer repo.Close()
    err = repo.Extract("hello", "world")
    if err != nil {
        log.Fatal(err)
    }
}
