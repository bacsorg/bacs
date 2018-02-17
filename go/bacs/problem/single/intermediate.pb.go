// Code generated by protoc-gen-go. DO NOT EDIT.
// source: bacs/problem/single/intermediate.proto

/*
Package single is a generated protocol buffer package.

It is generated from these files:
	bacs/problem/single/intermediate.proto
	bacs/problem/single/problem.proto
	bacs/problem/single/result.proto
	bacs/problem/single/task.proto
	bacs/problem/single/testing.proto

It has these top-level messages:
	IntermediateResult
	ProblemExtension
	Tests
	ProfileExtension
	Dependency
	TestGroup
	Result
	TestGroupResult
	TestResult
	JudgeResult
	FileResult
	Task
	TestQuery
	TestSequence
*/
package single

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion2 // please upgrade the proto package

type IntermediateResult_State int32

const (
	IntermediateResult_UNKNOWN     IntermediateResult_State = 0
	IntermediateResult_INITIALIZED IntermediateResult_State = 1
	IntermediateResult_BUILDING    IntermediateResult_State = 2
	IntermediateResult_TESTING     IntermediateResult_State = 3
)

var IntermediateResult_State_name = map[int32]string{
	0: "UNKNOWN",
	1: "INITIALIZED",
	2: "BUILDING",
	3: "TESTING",
}
var IntermediateResult_State_value = map[string]int32{
	"UNKNOWN":     0,
	"INITIALIZED": 1,
	"BUILDING":    2,
	"TESTING":     3,
}

func (x IntermediateResult_State) String() string {
	return proto.EnumName(IntermediateResult_State_name, int32(x))
}
func (IntermediateResult_State) EnumDescriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 0} }

type IntermediateResult struct {
	State       IntermediateResult_State `protobuf:"varint,1,opt,name=state,enum=bacs.problem.single.IntermediateResult_State" json:"state,omitempty"`
	TestGroupId string                   `protobuf:"bytes,2,opt,name=test_group_id,json=testGroupId" json:"test_group_id,omitempty"`
	TestId      string                   `protobuf:"bytes,3,opt,name=test_id,json=testId" json:"test_id,omitempty"`
}

func (m *IntermediateResult) Reset()                    { *m = IntermediateResult{} }
func (m *IntermediateResult) String() string            { return proto.CompactTextString(m) }
func (*IntermediateResult) ProtoMessage()               {}
func (*IntermediateResult) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

func (m *IntermediateResult) GetState() IntermediateResult_State {
	if m != nil {
		return m.State
	}
	return IntermediateResult_UNKNOWN
}

func (m *IntermediateResult) GetTestGroupId() string {
	if m != nil {
		return m.TestGroupId
	}
	return ""
}

func (m *IntermediateResult) GetTestId() string {
	if m != nil {
		return m.TestId
	}
	return ""
}

func init() {
	proto.RegisterType((*IntermediateResult)(nil), "bacs.problem.single.IntermediateResult")
	proto.RegisterEnum("bacs.problem.single.IntermediateResult_State", IntermediateResult_State_name, IntermediateResult_State_value)
}

func init() { proto.RegisterFile("bacs/problem/single/intermediate.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 238 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xe2, 0x52, 0x4b, 0x4a, 0x4c, 0x2e,
	0xd6, 0x2f, 0x28, 0xca, 0x4f, 0xca, 0x49, 0xcd, 0xd5, 0x2f, 0xce, 0xcc, 0x4b, 0xcf, 0x49, 0xd5,
	0xcf, 0xcc, 0x2b, 0x49, 0x2d, 0xca, 0x4d, 0x4d, 0xc9, 0x4c, 0x2c, 0x49, 0xd5, 0x2b, 0x28, 0xca,
	0x2f, 0xc9, 0x17, 0x12, 0x06, 0xa9, 0xd3, 0x83, 0xaa, 0xd3, 0x83, 0xa8, 0x53, 0xba, 0xc1, 0xc8,
	0x25, 0xe4, 0x89, 0xa4, 0x36, 0x28, 0xb5, 0xb8, 0x34, 0xa7, 0x44, 0xc8, 0x99, 0x8b, 0xb5, 0xb8,
	0x24, 0xb1, 0x24, 0x55, 0x82, 0x51, 0x81, 0x51, 0x83, 0xcf, 0x48, 0x57, 0x0f, 0x8b, 0x5e, 0x3d,
	0x4c, 0x7d, 0x7a, 0xc1, 0x20, 0x4d, 0x41, 0x10, 0xbd, 0x42, 0x4a, 0x5c, 0xbc, 0x25, 0xa9, 0xc5,
	0x25, 0xf1, 0xe9, 0x45, 0xf9, 0xa5, 0x05, 0xf1, 0x99, 0x29, 0x12, 0x4c, 0x0a, 0x8c, 0x1a, 0x9c,
	0x41, 0xdc, 0x20, 0x41, 0x77, 0x90, 0x98, 0x67, 0x8a, 0x90, 0x38, 0x17, 0x3b, 0x58, 0x4d, 0x66,
	0x8a, 0x04, 0x33, 0x58, 0x96, 0x0d, 0xc4, 0xf5, 0x4c, 0x51, 0x72, 0xe0, 0x62, 0x05, 0x1b, 0x26,
	0xc4, 0xcd, 0xc5, 0x1e, 0xea, 0xe7, 0xed, 0xe7, 0x1f, 0xee, 0x27, 0xc0, 0x20, 0xc4, 0xcf, 0xc5,
	0xed, 0xe9, 0xe7, 0x19, 0xe2, 0xe9, 0xe8, 0xe3, 0x19, 0xe5, 0xea, 0x22, 0xc0, 0x28, 0xc4, 0xc3,
	0xc5, 0xe1, 0x14, 0xea, 0xe9, 0xe3, 0xe2, 0xe9, 0xe7, 0x2e, 0xc0, 0x04, 0x52, 0x1b, 0xe2, 0x1a,
	0x1c, 0x02, 0xe2, 0x30, 0x3b, 0x71, 0x44, 0xb1, 0x41, 0x1c, 0x9a, 0xc4, 0x06, 0x0e, 0x00, 0x63,
	0x40, 0x00, 0x00, 0x00, 0xff, 0xff, 0x29, 0x33, 0xc3, 0x38, 0x2a, 0x01, 0x00, 0x00,
}
