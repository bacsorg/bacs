// Code generated by protoc-gen-go. DO NOT EDIT.
// source: bunsan/broker/worker/protocol/event.proto

/*
Package protocol is a generated protocol buffer package.

It is generated from these files:
	bunsan/broker/worker/protocol/event.proto

It has these top-level messages:
	Event
*/
package protocol

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import bunsan_broker "bunsan/broker"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion2 // please upgrade the proto package

type Event struct {
	// Types that are valid to be assigned to Kind:
	//	*Event_Status
	//	*Event_Result
	Kind isEvent_Kind `protobuf_oneof:"Kind"`
}

func (m *Event) Reset()                    { *m = Event{} }
func (m *Event) String() string            { return proto.CompactTextString(m) }
func (*Event) ProtoMessage()               {}
func (*Event) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

type isEvent_Kind interface {
	isEvent_Kind()
}

type Event_Status struct {
	Status *bunsan_broker.Status `protobuf:"bytes,1,opt,name=status,oneof"`
}
type Event_Result struct {
	Result *bunsan_broker.Result `protobuf:"bytes,2,opt,name=result,oneof"`
}

func (*Event_Status) isEvent_Kind() {}
func (*Event_Result) isEvent_Kind() {}

func (m *Event) GetKind() isEvent_Kind {
	if m != nil {
		return m.Kind
	}
	return nil
}

func (m *Event) GetStatus() *bunsan_broker.Status {
	if x, ok := m.GetKind().(*Event_Status); ok {
		return x.Status
	}
	return nil
}

func (m *Event) GetResult() *bunsan_broker.Result {
	if x, ok := m.GetKind().(*Event_Result); ok {
		return x.Result
	}
	return nil
}

// XXX_OneofFuncs is for the internal use of the proto package.
func (*Event) XXX_OneofFuncs() (func(msg proto.Message, b *proto.Buffer) error, func(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error), func(msg proto.Message) (n int), []interface{}) {
	return _Event_OneofMarshaler, _Event_OneofUnmarshaler, _Event_OneofSizer, []interface{}{
		(*Event_Status)(nil),
		(*Event_Result)(nil),
	}
}

func _Event_OneofMarshaler(msg proto.Message, b *proto.Buffer) error {
	m := msg.(*Event)
	// Kind
	switch x := m.Kind.(type) {
	case *Event_Status:
		b.EncodeVarint(1<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Status); err != nil {
			return err
		}
	case *Event_Result:
		b.EncodeVarint(2<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.Result); err != nil {
			return err
		}
	case nil:
	default:
		return fmt.Errorf("Event.Kind has unexpected type %T", x)
	}
	return nil
}

func _Event_OneofUnmarshaler(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error) {
	m := msg.(*Event)
	switch tag {
	case 1: // Kind.status
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(bunsan_broker.Status)
		err := b.DecodeMessage(msg)
		m.Kind = &Event_Status{msg}
		return true, err
	case 2: // Kind.result
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(bunsan_broker.Result)
		err := b.DecodeMessage(msg)
		m.Kind = &Event_Result{msg}
		return true, err
	default:
		return false, nil
	}
}

func _Event_OneofSizer(msg proto.Message) (n int) {
	m := msg.(*Event)
	// Kind
	switch x := m.Kind.(type) {
	case *Event_Status:
		s := proto.Size(x.Status)
		n += proto.SizeVarint(1<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case *Event_Result:
		s := proto.Size(x.Result)
		n += proto.SizeVarint(2<<3 | proto.WireBytes)
		n += proto.SizeVarint(uint64(s))
		n += s
	case nil:
	default:
		panic(fmt.Sprintf("proto: unexpected type %T in oneof", x))
	}
	return n
}

func init() {
	proto.RegisterType((*Event)(nil), "bunsan.broker.worker.protocol.Event")
}

func init() { proto.RegisterFile("bunsan/broker/worker/protocol/event.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 157 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xe2, 0xd2, 0x4c, 0x2a, 0xcd, 0x2b,
	0x4e, 0xcc, 0xd3, 0x4f, 0x2a, 0xca, 0xcf, 0x4e, 0x2d, 0xd2, 0x2f, 0xcf, 0x2f, 0x02, 0x51, 0x05,
	0x45, 0xf9, 0x25, 0xf9, 0xc9, 0xf9, 0x39, 0xfa, 0xa9, 0x65, 0xa9, 0x79, 0x25, 0x7a, 0x60, 0xae,
	0x90, 0x2c, 0x44, 0xa9, 0x1e, 0x44, 0xa9, 0x1e, 0x44, 0xa9, 0x1e, 0x4c, 0xa9, 0x94, 0x0c, 0xaa,
	0x49, 0x30, 0x71, 0x88, 0x02, 0xa5, 0x42, 0x2e, 0x56, 0x57, 0x90, 0x59, 0x42, 0xfa, 0x5c, 0x6c,
	0xc5, 0x25, 0x89, 0x25, 0xa5, 0xc5, 0x12, 0x8c, 0x0a, 0x8c, 0x1a, 0xdc, 0x46, 0xa2, 0x7a, 0xa8,
	0xc6, 0x06, 0x83, 0x25, 0x3d, 0x18, 0x82, 0xa0, 0xca, 0x40, 0x1a, 0x8a, 0x52, 0x8b, 0x4b, 0x73,
	0x4a, 0x24, 0x98, 0xb0, 0x6a, 0x08, 0x02, 0x4b, 0x82, 0x34, 0x40, 0x94, 0x39, 0xb1, 0x71, 0xb1,
	0x78, 0x67, 0xe6, 0xa5, 0x38, 0x71, 0x45, 0x71, 0xc0, 0x1c, 0x91, 0xc4, 0x06, 0x66, 0x19, 0x03,
	0x02, 0x00, 0x00, 0xff, 0xff, 0x33, 0x32, 0x53, 0x4e, 0xef, 0x00, 0x00, 0x00,
}
