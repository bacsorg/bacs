// Code generated by protoc-gen-go. DO NOT EDIT.
// source: bacs/problem/single/process/file.proto

package process

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import bacs_file "bacs/file"
import bacs_file1 "bacs/file"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

type File_Permissions int32

const (
	File_READ    File_Permissions = 0
	File_WRITE   File_Permissions = 1
	File_EXECUTE File_Permissions = 2
)

var File_Permissions_name = map[int32]string{
	0: "READ",
	1: "WRITE",
	2: "EXECUTE",
}
var File_Permissions_value = map[string]int32{
	"READ":    0,
	"WRITE":   1,
	"EXECUTE": 2,
}

func (x File_Permissions) String() string {
	return proto.EnumName(File_Permissions_name, int32(x))
}
func (File_Permissions) EnumDescriptor() ([]byte, []int) { return fileDescriptor1, []int{0, 0} }

type File struct {
	Id         string             `protobuf:"bytes,1,opt,name=id" json:"id,omitempty"`
	Permission []File_Permissions `protobuf:"varint,2,rep,packed,name=permission,enum=bacs.problem.single.process.File_Permissions" json:"permission,omitempty"`
	// DataId
	Init string `protobuf:"bytes,3,opt,name=init" json:"init,omitempty"`
	// arbitrary paths are not supported and stripped to filename
	Path    *bacs_file.Path   `protobuf:"bytes,4,opt,name=path" json:"path,omitempty"`
	Receive *bacs_file1.Range `protobuf:"bytes,5,opt,name=receive" json:"receive,omitempty"`
}

func (m *File) Reset()                    { *m = File{} }
func (m *File) String() string            { return proto.CompactTextString(m) }
func (*File) ProtoMessage()               {}
func (*File) Descriptor() ([]byte, []int) { return fileDescriptor1, []int{0} }

func (m *File) GetId() string {
	if m != nil {
		return m.Id
	}
	return ""
}

func (m *File) GetPermission() []File_Permissions {
	if m != nil {
		return m.Permission
	}
	return nil
}

func (m *File) GetInit() string {
	if m != nil {
		return m.Init
	}
	return ""
}

func (m *File) GetPath() *bacs_file.Path {
	if m != nil {
		return m.Path
	}
	return nil
}

func (m *File) GetReceive() *bacs_file1.Range {
	if m != nil {
		return m.Receive
	}
	return nil
}

func init() {
	proto.RegisterType((*File)(nil), "bacs.problem.single.process.File")
	proto.RegisterEnum("bacs.problem.single.process.File_Permissions", File_Permissions_name, File_Permissions_value)
}

func init() { proto.RegisterFile("bacs/problem/single/process/file.proto", fileDescriptor1) }

var fileDescriptor1 = []byte{
	// 268 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x90, 0x4d, 0x4b, 0xc3, 0x40,
	0x10, 0x86, 0x4d, 0x9a, 0x1a, 0x33, 0x81, 0x1a, 0x06, 0x85, 0x50, 0x2f, 0xa1, 0x82, 0x04, 0xc1,
	0x0d, 0xd4, 0x5f, 0xe0, 0xc7, 0x0a, 0x1e, 0x84, 0xb2, 0x54, 0x14, 0x6f, 0x49, 0xba, 0xb6, 0x03,
	0x69, 0x12, 0x76, 0x83, 0x7f, 0xde, 0x8b, 0xec, 0x26, 0x6a, 0x4e, 0xbd, 0xed, 0xc7, 0x33, 0xcf,
	0x3b, 0x33, 0x70, 0x55, 0xe4, 0xa5, 0xce, 0x5a, 0xd5, 0x14, 0x95, 0xdc, 0x67, 0x9a, 0xea, 0x6d,
	0x25, 0xcd, 0xb5, 0x94, 0x5a, 0x67, 0x9f, 0x54, 0x49, 0xd6, 0xaa, 0xa6, 0x6b, 0xf0, 0xc2, 0x70,
	0x6c, 0xe0, 0x58, 0xcf, 0xb1, 0x81, 0x9b, 0x9f, 0x59, 0x89, 0xa1, 0xb3, 0x36, 0xef, 0x76, 0x7d,
	0xc9, 0xfc, 0xfc, 0xff, 0x55, 0xe5, 0xf5, 0x76, 0x30, 0x2d, 0xbe, 0x1d, 0xf0, 0x9e, 0xa8, 0x92,
	0x38, 0x03, 0x97, 0x36, 0xb1, 0x93, 0x38, 0x69, 0x20, 0x5c, 0xda, 0xe0, 0x0b, 0x40, 0x2b, 0xd5,
	0x9e, 0xb4, 0xa6, 0xa6, 0x8e, 0xdd, 0x64, 0x92, 0xce, 0x96, 0x37, 0xec, 0x40, 0x2e, 0x33, 0x1a,
	0xb6, 0xfa, 0xab, 0xd1, 0x62, 0x24, 0x40, 0x04, 0x8f, 0x6a, 0xea, 0xe2, 0x89, 0x0d, 0xb0, 0x67,
	0xbc, 0x04, 0xcf, 0x34, 0x18, 0x7b, 0x89, 0x93, 0x86, 0xcb, 0xd3, 0x5e, 0x6e, 0xa7, 0x5c, 0xe5,
	0xdd, 0x4e, 0xd8, 0x4f, 0xbc, 0x06, 0x5f, 0xc9, 0x52, 0xd2, 0x97, 0x8c, 0xa7, 0x96, 0x8b, 0x46,
	0x9c, 0x30, 0x93, 0x88, 0x5f, 0x60, 0x91, 0x41, 0x38, 0xca, 0xc7, 0x13, 0xf0, 0x04, 0xbf, 0x7b,
	0x8c, 0x8e, 0x30, 0x80, 0xe9, 0x9b, 0x78, 0x5e, 0xf3, 0xc8, 0xc1, 0x10, 0x7c, 0xfe, 0xce, 0x1f,
	0x5e, 0xd7, 0x3c, 0x72, 0xef, 0x83, 0x0f, 0x7f, 0xe8, 0xbe, 0x38, 0xb6, 0xfb, 0xb8, 0xfd, 0x09,
	0x00, 0x00, 0xff, 0xff, 0xd3, 0x6c, 0x96, 0x7e, 0x83, 0x01, 0x00, 0x00,
}
