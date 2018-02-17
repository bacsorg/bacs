// Code generated by protoc-gen-go. DO NOT EDIT.
// source: bacs/archive/problem/error.proto

/*
Package problem is a generated protocol buffer package.

It is generated from these files:
	bacs/archive/problem/error.proto
	bacs/archive/problem/flag.proto
	bacs/archive/problem/id.proto
	bacs/archive/problem/import.proto
	bacs/archive/problem/status.proto

It has these top-level messages:
	Error
	FlagSet
	Flag
	IdSet
	ImportResult
	ImportMap
	Status
	StatusResult
	StatusMap
*/
package problem

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

type Error_Code int32

const (
	Error_UNKNOWN        Error_Code = 0
	Error_NOT_FOUND      Error_Code = 1
	Error_LOCKED         Error_Code = 2
	Error_READ_ONLY      Error_Code = 3
	Error_PENDING_IMPORT Error_Code = 4
	Error_INVALID_FORMAT Error_Code = 5
)

var Error_Code_name = map[int32]string{
	0: "UNKNOWN",
	1: "NOT_FOUND",
	2: "LOCKED",
	3: "READ_ONLY",
	4: "PENDING_IMPORT",
	5: "INVALID_FORMAT",
}
var Error_Code_value = map[string]int32{
	"UNKNOWN":        0,
	"NOT_FOUND":      1,
	"LOCKED":         2,
	"READ_ONLY":      3,
	"PENDING_IMPORT": 4,
	"INVALID_FORMAT": 5,
}

func (x Error_Code) String() string {
	return proto.EnumName(Error_Code_name, int32(x))
}
func (Error_Code) EnumDescriptor() ([]byte, []int) { return fileDescriptor0, []int{0, 0} }

type Error struct {
	Code   Error_Code `protobuf:"varint,1,opt,name=code,enum=bacs.archive.problem.Error_Code" json:"code,omitempty"`
	Reason string     `protobuf:"bytes,2,opt,name=reason" json:"reason,omitempty"`
}

func (m *Error) Reset()                    { *m = Error{} }
func (m *Error) String() string            { return proto.CompactTextString(m) }
func (*Error) ProtoMessage()               {}
func (*Error) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

func (m *Error) GetCode() Error_Code {
	if m != nil {
		return m.Code
	}
	return Error_UNKNOWN
}

func (m *Error) GetReason() string {
	if m != nil {
		return m.Reason
	}
	return ""
}

func init() {
	proto.RegisterType((*Error)(nil), "bacs.archive.problem.Error")
	proto.RegisterEnum("bacs.archive.problem.Error_Code", Error_Code_name, Error_Code_value)
}

func init() { proto.RegisterFile("bacs/archive/problem/error.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 230 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x6c, 0xcf, 0xc1, 0x4a, 0xc3, 0x40,
	0x10, 0x06, 0x60, 0xb7, 0xa6, 0x29, 0x19, 0xb1, 0x2c, 0x83, 0x48, 0x8f, 0xa1, 0xa7, 0x9e, 0x36,
	0xa0, 0xbe, 0x40, 0xec, 0xa6, 0x12, 0x9a, 0xce, 0x96, 0x25, 0x55, 0xf4, 0x12, 0x92, 0x74, 0x41,
	0x41, 0xdd, 0xb2, 0x11, 0x9f, 0xce, 0x87, 0x93, 0x0d, 0x7b, 0xec, 0x71, 0xe6, 0xff, 0x0e, 0xff,
	0x0f, 0x69, 0xd7, 0xf6, 0x43, 0xd6, 0xba, 0xfe, 0xfd, 0xe3, 0xd7, 0x64, 0x27, 0x67, 0xbb, 0x4f,
	0xf3, 0x95, 0x19, 0xe7, 0xac, 0x13, 0x27, 0x67, 0x7f, 0x2c, 0xde, 0x78, 0x21, 0x82, 0x10, 0x41,
	0x2c, 0xff, 0x18, 0x4c, 0x0b, 0xaf, 0xf0, 0x01, 0xa2, 0xde, 0x1e, 0xcd, 0x82, 0xa5, 0x6c, 0x35,
	0xbf, 0x4b, 0xc5, 0x39, 0x2e, 0x46, 0x2a, 0xd6, 0xf6, 0x68, 0xf4, 0xa8, 0xf1, 0x16, 0x62, 0x67,
	0xda, 0xc1, 0x7e, 0x2f, 0x26, 0x29, 0x5b, 0x25, 0x3a, 0x5c, 0x4b, 0x03, 0x91, 0x57, 0x78, 0x05,
	0xb3, 0x03, 0x6d, 0x49, 0xbd, 0x10, 0xbf, 0xc0, 0x6b, 0x48, 0x48, 0xd5, 0xcd, 0x46, 0x1d, 0x48,
	0x72, 0x86, 0x00, 0x71, 0xa5, 0xd6, 0xdb, 0x42, 0xf2, 0x89, 0x8f, 0x74, 0x91, 0xcb, 0x46, 0x51,
	0xf5, 0xca, 0x2f, 0x11, 0x61, 0xbe, 0x2f, 0x48, 0x96, 0xf4, 0xd4, 0x94, 0xbb, 0xbd, 0xd2, 0x35,
	0x8f, 0xfc, 0xaf, 0xa4, 0xe7, 0xbc, 0x2a, 0x65, 0xb3, 0x51, 0x7a, 0x97, 0xd7, 0x7c, 0xfa, 0x98,
	0xbc, 0xcd, 0x42, 0xb5, 0x2e, 0x1e, 0x67, 0xde, 0xff, 0x07, 0x00, 0x00, 0xff, 0xff, 0x60, 0xbc,
	0x14, 0x0f, 0x0a, 0x01, 0x00, 0x00,
}
