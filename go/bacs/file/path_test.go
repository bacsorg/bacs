package file

import (
    "testing"

    "github.com/stretchr/testify/assert"
)

func TestGoPath(t *testing.T) {
    testData := []struct {
        p        Path
        expected string
    }{
        {
            Path{},
            "",
        },
        {
            Path{
                Element: []string{
                    "hello",
                    "world",
                },
            },
            "hello/world",
        },
        {
            Path{
                Root: "/",
                Element: []string{
                    "hello",
                    "world",
                },
            },
            "/hello/world",
        },
    }
    for _, tt := range testData {
        assert.Equal(t, tt.expected, tt.p.GoPath())
    }
}
