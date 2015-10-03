// Code generated by protoc-gen-go.
// source: bacs/external/language.proto
// DO NOT EDIT!

package bacs_external

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

type Language struct {
	Id   *Language_Id              `protobuf:"bytes,1,opt,name=id" json:"id,omitempty"`
	Info map[string]*Language_Info `protobuf:"bytes,2,rep,name=info" json:"info,omitempty" protobuf_key:"bytes,1,opt,name=key" protobuf_val:"bytes,2,opt,name=value"`
}

func (m *Language) Reset()         { *m = Language{} }
func (m *Language) String() string { return proto.CompactTextString(m) }
func (*Language) ProtoMessage()    {}

func (m *Language) GetId() *Language_Id {
	if m != nil {
		return m.Id
	}
	return nil
}

func (m *Language) GetInfo() map[string]*Language_Info {
	if m != nil {
		return m.Info
	}
	return nil
}

type Language_Id struct {
	Value string `protobuf:"bytes,1,opt,name=value" json:"value,omitempty"`
}

func (m *Language_Id) Reset()         { *m = Language_Id{} }
func (m *Language_Id) String() string { return proto.CompactTextString(m) }
func (*Language_Id) ProtoMessage()    {}

type Language_Info struct {
	Name        string `protobuf:"bytes,1,opt,name=name" json:"name,omitempty"`
	Description string `protobuf:"bytes,2,opt,name=description" json:"description,omitempty"`
}

func (m *Language_Info) Reset()         { *m = Language_Info{} }
func (m *Language_Info) String() string { return proto.CompactTextString(m) }
func (*Language_Info) ProtoMessage()    {}

type LanguageList struct {
	Entry []*Language `protobuf:"bytes,1,rep,name=entry" json:"entry,omitempty"`
}

func (m *LanguageList) Reset()         { *m = LanguageList{} }
func (m *LanguageList) String() string { return proto.CompactTextString(m) }
func (*LanguageList) ProtoMessage()    {}

func (m *LanguageList) GetEntry() []*Language {
	if m != nil {
		return m.Entry
	}
	return nil
}
