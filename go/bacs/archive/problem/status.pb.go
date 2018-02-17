// Code generated by protoc-gen-go. DO NOT EDIT.
// source: bacs/archive/problem/status.proto

package problem

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import bacs_problem "bacs/problem"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

type Status struct {
	Flags    *FlagSet               `protobuf:"bytes,1,opt,name=flags" json:"flags,omitempty"`
	Revision *bacs_problem.Revision `protobuf:"bytes,2,opt,name=revision" json:"revision,omitempty"`
}

func (m *Status) Reset()                    { *m = Status{} }
func (m *Status) String() string            { return proto.CompactTextString(m) }
func (*Status) ProtoMessage()               {}
func (*Status) Descriptor() ([]byte, []int) { return fileDescriptor4, []int{0} }

func (m *Status) GetFlags() *FlagSet {
	if m != nil {
		return m.Flags
	}
	return nil
}

func (m *Status) GetRevision() *bacs_problem.Revision {
	if m != nil {
		return m.Revision
	}
	return nil
}

type StatusResult struct {
	// Types that are valid to be assigned to Result:
	//	*StatusResult_Error
	//	*StatusResult_Status
	Result isStatusResult_Result `protobuf_oneof:"result"`
}

func (m *StatusResult) Reset()                    { *m = StatusResult{} }
func (m *StatusResult) String() string            { return proto.CompactTextString(m) }
func (*StatusResult) ProtoMessage()               {}
func (*StatusResult) Descriptor() ([]byte, []int) { return fileDescriptor4, []int{1} }

type isStatusResult_Result interface {
	isStatusResult_Result()
}

type StatusResult_Error struct {
	Error *Error `protobuf:"bytes,1,opt,name=error,oneof"`
}
type StatusResult_Status struct {
	Status *Status `protobuf:"bytes,2,opt,name=status,oneof"`
}

func (*StatusResult_Error) isStatusResult_Result()  {}
func (*StatusResult_Status) isStatusResult_Result() {}

func (m *StatusResult) GetResult() isStatusResult_Result {
	if m != nil {
		return m.Result
	}
	return nil
}

func (m *StatusResult) GetError() *Error {
	if x, ok := m.GetResult().(*StatusResult_Error); ok {
		return x.Error
	}
	return nil
}

func (m *StatusResult) GetStatus() *Status {
	if x, ok := m.GetResult().(*StatusResult_Status); ok {
		return x.Status
	}
	return nil
}

// XXX_OneofFuncs is for the internal use of the proto package.
func (*StatusResult) XXX_OneofFuncs() (func(msg proto.Message, b *proto.Buffer) error, func(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error), func(msg proto.Message) (n int), []interface{}) {
	return _StatusResult_OneofMarshaler, _StatusResult_OneofUnmarshaler, _StatusResult_OneofSizer, []interface{}{
		(*StatusResult_Error)(nil),
		(*StatusResult_Status)(nil),
	}
}

func _StatusResult_OneofMarshaler(msg proto.Message, b *proto.Buffer) error {
	m := msg.(*StatusResult)
	// result
	switch x := m.Result.(type) {
	case *StatusResult_Error:
		b.EncodeVarint(1<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Error); err != nil {
			return err
		}
	case *StatusResult_Status:
		b.EncodeVarint(2<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Status); err != nil {
			return err
		}
	case nil:
	default:
		return fmt.Errorf("StatusResult.Result has unexpected type %T", x)
	}
	return nil
}

func _StatusResult_OneofUnmarshaler(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error) {
	m := msg.(*StatusResult)
	switch tag {
	case 1: // result.error
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Error)
		err := b.DecodeMessage(msg)
		m.Result = &StatusResult_Error{msg}
		return true, err
	case 2: // result.status
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(Status)
		err := b.DecodeMessage(msg)
		m.Result = &StatusResult_Status{msg}
		return true, err
	default:
		return false, nil
	}
}

func _StatusResult_OneofSizer(msg proto.Message) (n int) {
	m := msg.(*StatusResult)
	// result
	switch x := m.Result.(type) {
	case *StatusResult_Error:
		s := proto.Size(x.Error)
		n += proto.SizeVarint(1<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *StatusResult_Status:
		s := proto.Size(x.Status)
		n += proto.SizeVarint(2<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case nil:
	default:
		panic(fmt.Sprintf("proto: unexpected type %T in oneof", x))
	}
	return n
}

type StatusMap struct {
	Entry map[string]*StatusResult `protobuf:"bytes,1,rep,name=entry" json:"entry,omitempty" protobuf_key:"bytes,1,opt,name=key" protobuf_val:"bytes,2,opt,name=value"`
}

func (m *StatusMap) Reset()                    { *m = StatusMap{} }
func (m *StatusMap) String() string            { return proto.CompactTextString(m) }
func (*StatusMap) ProtoMessage()               {}
func (*StatusMap) Descriptor() ([]byte, []int) { return fileDescriptor4, []int{2} }

func (m *StatusMap) GetEntry() map[string]*StatusResult {
	if m != nil {
		return m.Entry
	}
	return nil
}

func init() {
	proto.RegisterType((*Status)(nil), "bacs.archive.problem.Status")
	proto.RegisterType((*StatusResult)(nil), "bacs.archive.problem.StatusResult")
	proto.RegisterType((*StatusMap)(nil), "bacs.archive.problem.StatusMap")
}

func init() { proto.RegisterFile("bacs/archive/problem/status.proto", fileDescriptor4) }

var fileDescriptor4 = []byte{
	// 307 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x91, 0x41, 0x4b, 0xf3, 0x30,
	0x18, 0xc7, 0x97, 0x8d, 0xf5, 0xdd, 0x9e, 0xbd, 0x07, 0x09, 0x22, 0xa5, 0x2a, 0xd6, 0x9e, 0x86,
	0x87, 0x14, 0x3a, 0x90, 0xe1, 0x49, 0x06, 0x93, 0x5d, 0x76, 0xc9, 0x6e, 0xe2, 0x25, 0x1d, 0x71,
	0x16, 0xeb, 0x5a, 0x93, 0xb4, 0xb0, 0x0f, 0xe0, 0xa7, 0xf1, 0x4b, 0x4a, 0xf2, 0x64, 0x7a, 0x29,
	0xbb, 0xb4, 0xa5, 0xff, 0xdf, 0xbf, 0xbf, 0x27, 0x4f, 0xe1, 0x36, 0x17, 0x5b, 0x9d, 0x0a, 0xb5,
	0x7d, 0x2b, 0x5a, 0x99, 0xd6, 0xaa, 0xca, 0x4b, 0xf9, 0x91, 0x6a, 0x23, 0x4c, 0xa3, 0x59, 0xad,
	0x2a, 0x53, 0xd1, 0x73, 0x8b, 0x30, 0x8f, 0x30, 0x8f, 0x44, 0x71, 0x67, 0x51, 0x2a, 0x55, 0x29,
	0xec, 0x45, 0x37, 0x9d, 0xc4, 0x6b, 0x29, 0x76, 0x1e, 0x88, 0x1c, 0x70, 0x0c, 0xfc, 0x1d, 0xb3,
	0xe4, 0x13, 0x82, 0x8d, 0x1b, 0x82, 0xce, 0x60, 0x68, 0x3b, 0x3a, 0x24, 0x31, 0x99, 0x4e, 0xb2,
	0x6b, 0xd6, 0x35, 0x0e, 0x7b, 0x2a, 0xc5, 0x6e, 0x23, 0x0d, 0x47, 0x96, 0x66, 0x30, 0x52, 0xb2,
	0x2d, 0x74, 0x51, 0xed, 0xc3, 0xbe, 0xeb, 0x5d, 0x60, 0xef, 0xc8, 0x73, 0x9f, 0xf2, 0x5f, 0x2e,
	0xf9, 0x22, 0xf0, 0x1f, 0x9d, 0x5c, 0xea, 0xa6, 0x34, 0xd6, 0xec, 0xce, 0xe3, 0xcd, 0x97, 0xdd,
	0xe6, 0xa5, 0x45, 0x56, 0x3d, 0x8e, 0x2c, 0xbd, 0x87, 0x00, 0xb7, 0xe7, 0xbd, 0x57, 0xdd, 0x2d,
	0x14, 0xad, 0x7a, 0xdc, 0xd3, 0x8b, 0x11, 0x04, 0xca, 0x69, 0x93, 0x6f, 0x02, 0x63, 0x8c, 0xd7,
	0xa2, 0xa6, 0x8f, 0x30, 0x94, 0x7b, 0xa3, 0x0e, 0x21, 0x89, 0x07, 0xd3, 0x49, 0x76, 0x77, 0xea,
	0x73, 0x6b, 0x51, 0xb3, 0xa5, 0x85, 0xdd, 0x85, 0x63, 0x31, 0x7a, 0x01, 0xf8, 0x7b, 0x49, 0xcf,
	0x60, 0xf0, 0x2e, 0x0f, 0xee, 0x48, 0x63, 0x6e, 0x1f, 0xe9, 0x1c, 0x86, 0xad, 0x28, 0x1b, 0xe9,
	0x07, 0x4e, 0x4e, 0x19, 0x70, 0x33, 0x1c, 0x0b, 0x0f, 0xfd, 0x39, 0x59, 0x8c, 0x9f, 0xff, 0x79,
	0x24, 0x0f, 0xdc, 0xaf, 0x9b, 0xfd, 0x04, 0x00, 0x00, 0xff, 0xff, 0x3d, 0x73, 0x33, 0x86, 0x54,
	0x02, 0x00, 0x00,
}
