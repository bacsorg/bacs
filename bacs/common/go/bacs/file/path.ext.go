package file

import (
    "path"
)

func (p *Path) GoPath() string {
    go_path := p.Root
    for _, element := range p.Element {
        go_path = path.Join(go_path, element)
    }
    return go_path
}
