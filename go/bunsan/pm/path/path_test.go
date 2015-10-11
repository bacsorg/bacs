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

func TestAbsolute(t *testing.T) {
    testData := []struct {
        path     string
        root     string
        absolute string
    }{
        {
            path:     "regular/path",
            root:     "",
            absolute: "regular/path",
        },
        {
            path:     "./relative/path",
            root:     "some",
            absolute: "some/relative/path",
        },
        {
            path:     "regular/path",
            root:     "non/empty/root",
            absolute: "regular/path",
        },
        {
            path:     "regular/path/with/..",
            root:     "",
            absolute: "regular/path",
        },
        {
            path:     "regular/path/./with/../dots",
            root:     "some/root",
            absolute: "regular/path/dots",
        },
        {
            path:     "../regular/path",
            root:     "some/root",
            absolute: "some/regular/path",
        },
    }
    for _, tt := range testData {
        assert.Equal(t, tt.absolute, Absolute(tt.path, tt.root))
    }
}
