package path

import (
    "testing"

    "github.com/stretchr/testify/assert"
)

func TestJoinSplit(t *testing.T) {
    testData := []struct {
        elements []string
        whole    string
        split    []string
    }{
        {
            elements: []string{"hello", "world"},
            whole:    "hello/world",
            split:    nil,
        },
        {
            elements: []string{},
            whole:    "",
            split:    nil,
        },
        {
            elements: []string{"single"},
            whole:    "single",
            split:    nil,
        },
        {
            elements: []string{"compound/elem", "other/one"},
            whole:    "compound/elem/other/one",
            split:    []string{"compound", "elem", "other", "one"},
        },
        {
            elements: []string{"use", "..", "and", ".", "here"},
            whole:    "use/../and/./here",
            split:    nil,
        },
        {
            elements: []string{".", "another", "..", "special", "value"},
            whole:    "./another/../special/value",
            split:    nil,
        },
    }
    for _, tt := range testData {
        split := tt.split
        if split == nil {
            split = tt.elements
        }
        assert.Equal(t, tt.whole, Join(tt.elements...))
        assert.Equal(t, Split(tt.whole), split)
    }
}
