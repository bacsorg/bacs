package local

import (
	"bytes"
	"encoding/json"
	"sort"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestIndex(t *testing.T) {
	testData := []struct {
		data    string
		index   Index
		all     []string
		sources []string
	}{
		{
			data: `{
				"source": {
					"import": {
						"source": [
							{"path": "path1", "package": "package1"},
							{"path": "path2", "package": "package2"}
						],
						"package": [
							{"path": "path3", "package": "package3"},
							{"path": "path4", "package": "package4"}
						]
					},
					"self": [
						{"path": "path5", "source": "source5"},
						{"path": "path6", "source": "source6"}
					]
				},
				"package": {
					"import": {
						"source": [
							{"path": "path7", "package": "package7"},
							{"path": "path8", "package": "package8"}
						],
						"package": [
							{"path": "path9", "package": "package9"},
							{"path": "path10", "package": "package10"}
						]
					},
					"self": [
						{"path": "path11", "source": "source11"},
						{"path": "path12", "source": "source12"}
					]
				}
			}`,
			index: Index{
				Source: PackageStage{
					Import: ExternalImports{
						Source: []TreeImport{
							{"path1", "package1"},
							{"path2", "package2"},
						},
						Package: []TreeImport{
							{"path3", "package3"},
							{"path4", "package4"},
						},
					},
					Self: []LocalImport{
						{"path5", "source5"},
						{"path6", "source6"},
					},
				},
				Package: PackageStage{
					Import: ExternalImports{
						Source: []TreeImport{
							{"path7", "package7"},
							{"path8", "package8"},
						},
						Package: []TreeImport{
							{"path9", "package9"},
							{"path10", "package10"},
						},
					},
					Self: []LocalImport{
						{"path11", "source11"},
						{"path12", "source12"},
					},
				},
			},
			all: []string{
				"package1",
				"package2",
				"package3",
				"package4",
				"package7",
				"package8",
				"package9",
				"package10",
			},
			sources: []string{
				"source5",
				"source6",
				"source11",
				"source12",
			},
		},
	}
	for _, tt := range testData {
		{ // read
			var index Index
			err := index.Unmarshal([]byte(tt.data))
			assert.NoError(t, err)
			assert.Equal(t, tt.index, index)
		}
		{ // marshal
			data, err := tt.index.Marshal()
			assert.NoError(t, err)
			var actual, expected bytes.Buffer
			json.Indent(&expected, []byte(tt.data), "", "")
			json.Indent(&actual, data, "", "")
			assert.Equal(t, string(expected.Bytes()), string(actual.Bytes()))
		}
		{ // all
			actual := tt.index.All()
			sort.Strings(actual)
			sort.Strings(tt.all)
			assert.Equal(t, tt.all, actual)
		}
		{ // sources
			actual := tt.index.Sources()
			sort.Strings(actual)
			sort.Strings(tt.sources)
			assert.Equal(t, tt.sources, actual)
		}
	}
}
