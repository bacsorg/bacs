package main

import "flag"
import "fmt"
import "go/parser"
import "go/token"
import "log"

var source = flag.String("source", "", "Source file to parse")

func main() {
    flag.Parse()
    fset := token.NewFileSet()
    file, err := parser.ParseFile(fset, *source, nil, parser.ImportsOnly)
    if err != nil {
        log.Fatal(err)
    }
    for _, imp := range file.Imports {
        fmt.Println(imp.Path.Value)
    }
}
