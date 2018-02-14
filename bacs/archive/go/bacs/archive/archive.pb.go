// Code generated by protoc-gen-go.
// source: bacs/archive/archive.proto
// DO NOT EDIT!

/*
Package archive is a generated protocol buffer package.

It is generated from these files:
	bacs/archive/archive.proto

It has these top-level messages:
	Chunk
	DownloadRequest
	RenameRequest
	ChangeFlagsRequest
	ArchiveServiceConfig
	ArchiveServerConfig
*/
package archive

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import bacs_archive_problem "github.com/bacsorg/archive/go/bacs/archive/problem"
import bacs_archive_problem1 "github.com/bacsorg/archive/go/bacs/archive/problem"
import bacs_archive_problem3 "github.com/bacsorg/archive/go/bacs/archive/problem"
import bacs_archive_problem4 "github.com/bacsorg/archive/go/bacs/archive/problem"
import bacs_utility "github.com/bacsorg/common/go/bacs/utility"
import google_protobuf1 "github.com/bunsanorg/protoutils/google/protobuf"

import (
	context "golang.org/x/net/context"
	grpc "google.golang.org/grpc"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

type Chunk struct {
	Format *bacs_utility.Archiver `protobuf:"bytes,1,opt,name=format" json:"format,omitempty"`
	Data   []byte                 `protobuf:"bytes,2,opt,name=data,proto3" json:"data,omitempty"`
}

func (m *Chunk) Reset()         { *m = Chunk{} }
func (m *Chunk) String() string { return proto.CompactTextString(m) }
func (*Chunk) ProtoMessage()    {}

func (m *Chunk) GetFormat() *bacs_utility.Archiver {
	if m != nil {
		return m.Format
	}
	return nil
}

type DownloadRequest struct {
	Ids    *bacs_archive_problem1.IdSet `protobuf:"bytes,1,opt,name=ids" json:"ids,omitempty"`
	Format *bacs_utility.Archiver       `protobuf:"bytes,2,opt,name=format" json:"format,omitempty"`
}

func (m *DownloadRequest) Reset()         { *m = DownloadRequest{} }
func (m *DownloadRequest) String() string { return proto.CompactTextString(m) }
func (*DownloadRequest) ProtoMessage()    {}

func (m *DownloadRequest) GetIds() *bacs_archive_problem1.IdSet {
	if m != nil {
		return m.Ids
	}
	return nil
}

func (m *DownloadRequest) GetFormat() *bacs_utility.Archiver {
	if m != nil {
		return m.Format
	}
	return nil
}

type RenameRequest struct {
	From string `protobuf:"bytes,1,opt,name=from" json:"from,omitempty"`
	To   string `protobuf:"bytes,2,opt,name=to" json:"to,omitempty"`
}

func (m *RenameRequest) Reset()         { *m = RenameRequest{} }
func (m *RenameRequest) String() string { return proto.CompactTextString(m) }
func (*RenameRequest) ProtoMessage()    {}

type ChangeFlagsRequest struct {
	Ids  *bacs_archive_problem1.IdSet  `protobuf:"bytes,1,opt,name=ids" json:"ids,omitempty"`
	Flag *bacs_archive_problem.FlagSet `protobuf:"bytes,2,opt,name=flag" json:"flag,omitempty"`
}

func (m *ChangeFlagsRequest) Reset()         { *m = ChangeFlagsRequest{} }
func (m *ChangeFlagsRequest) String() string { return proto.CompactTextString(m) }
func (*ChangeFlagsRequest) ProtoMessage()    {}

func (m *ChangeFlagsRequest) GetIds() *bacs_archive_problem1.IdSet {
	if m != nil {
		return m.Ids
	}
	return nil
}

func (m *ChangeFlagsRequest) GetFlag() *bacs_archive_problem.FlagSet {
	if m != nil {
		return m.Flag
	}
	return nil
}

// Configuration
type ArchiveServiceConfig struct {
	UploadDirectory string                            `protobuf:"bytes,1,opt,name=upload_directory" json:"upload_directory,omitempty"`
	Permissions     *ArchiveServiceConfig_Permissions `protobuf:"bytes,2,opt,name=permissions" json:"permissions,omitempty"`
}

func (m *ArchiveServiceConfig) Reset()         { *m = ArchiveServiceConfig{} }
func (m *ArchiveServiceConfig) String() string { return proto.CompactTextString(m) }
func (*ArchiveServiceConfig) ProtoMessage()    {}

func (m *ArchiveServiceConfig) GetPermissions() *ArchiveServiceConfig_Permissions {
	if m != nil {
		return m.Permissions
	}
	return nil
}

type ArchiveServiceConfig_Permissions struct {
	AllowAny    bool                                     `protobuf:"varint,1,opt,name=allow_any" json:"allow_any,omitempty"`
	AllowedUser []*ArchiveServiceConfig_Permissions_User `protobuf:"bytes,2,rep,name=allowed_user" json:"allowed_user,omitempty"`
}

func (m *ArchiveServiceConfig_Permissions) Reset()         { *m = ArchiveServiceConfig_Permissions{} }
func (m *ArchiveServiceConfig_Permissions) String() string { return proto.CompactTextString(m) }
func (*ArchiveServiceConfig_Permissions) ProtoMessage()    {}

func (m *ArchiveServiceConfig_Permissions) GetAllowedUser() []*ArchiveServiceConfig_Permissions_User {
	if m != nil {
		return m.AllowedUser
	}
	return nil
}

type ArchiveServiceConfig_Permissions_User struct {
	Identity []string `protobuf:"bytes,1,rep,name=identity" json:"identity,omitempty"`
}

func (m *ArchiveServiceConfig_Permissions_User) Reset()         { *m = ArchiveServiceConfig_Permissions_User{} }
func (m *ArchiveServiceConfig_Permissions_User) String() string { return proto.CompactTextString(m) }
func (*ArchiveServiceConfig_Permissions_User) ProtoMessage()    {}

type ArchiveServerConfig struct {
	Listen  string                `protobuf:"bytes,1,opt,name=listen" json:"listen,omitempty"`
	Service *ArchiveServiceConfig `protobuf:"bytes,2,opt,name=service" json:"service,omitempty"`
	// Types that are valid to be assigned to Credentials:
	//	*ArchiveServerConfig_SslCredentials_
	Credentials isArchiveServerConfig_Credentials `protobuf_oneof:"credentials"`
}

func (m *ArchiveServerConfig) Reset()         { *m = ArchiveServerConfig{} }
func (m *ArchiveServerConfig) String() string { return proto.CompactTextString(m) }
func (*ArchiveServerConfig) ProtoMessage()    {}

type isArchiveServerConfig_Credentials interface {
	isArchiveServerConfig_Credentials()
}

type ArchiveServerConfig_SslCredentials_ struct {
	SslCredentials *ArchiveServerConfig_SslCredentials `protobuf:"bytes,3,opt,name=ssl_credentials,oneof"`
}

func (*ArchiveServerConfig_SslCredentials_) isArchiveServerConfig_Credentials() {}

func (m *ArchiveServerConfig) GetCredentials() isArchiveServerConfig_Credentials {
	if m != nil {
		return m.Credentials
	}
	return nil
}

func (m *ArchiveServerConfig) GetService() *ArchiveServiceConfig {
	if m != nil {
		return m.Service
	}
	return nil
}

func (m *ArchiveServerConfig) GetSslCredentials() *ArchiveServerConfig_SslCredentials {
	if x, ok := m.GetCredentials().(*ArchiveServerConfig_SslCredentials_); ok {
		return x.SslCredentials
	}
	return nil
}

// XXX_OneofFuncs is for the internal use of the proto package.
func (*ArchiveServerConfig) XXX_OneofFuncs() (func(msg proto.Message, b *proto.Buffer) error, func(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error), []interface{}) {
	return _ArchiveServerConfig_OneofMarshaler, _ArchiveServerConfig_OneofUnmarshaler, []interface{}{
		(*ArchiveServerConfig_SslCredentials_)(nil),
	}
}

func _ArchiveServerConfig_OneofMarshaler(msg proto.Message, b *proto.Buffer) error {
	m := msg.(*ArchiveServerConfig)
	// credentials
	switch x := m.Credentials.(type) {
	case *ArchiveServerConfig_SslCredentials_:
		b.EncodeVarint(3<<3 | proto.WireBytes)
		if err := b.EncodeMessage(x.SslCredentials); err != nil {
			return err
		}
	case nil:
	default:
		return fmt.Errorf("ArchiveServerConfig.Credentials has unexpected type %T", x)
	}
	return nil
}

func _ArchiveServerConfig_OneofUnmarshaler(msg proto.Message, tag, wire int, b *proto.Buffer) (bool, error) {
	m := msg.(*ArchiveServerConfig)
	switch tag {
	case 3: // credentials.ssl_credentials
		if wire != proto.WireBytes {
			return true, proto.ErrInternalBadWireType
		}
		msg := new(ArchiveServerConfig_SslCredentials)
		err := b.DecodeMessage(msg)
		m.Credentials = &ArchiveServerConfig_SslCredentials_{msg}
		return true, err
	default:
		return false, nil
	}
}

type ArchiveServerConfig_SslCredentials struct {
	PemRootCertsPath string                                               `protobuf:"bytes,1,opt,name=pem_root_certs_path" json:"pem_root_certs_path,omitempty"`
	PemKeyCertPair   []*ArchiveServerConfig_SslCredentials_PemKeyCertPair `protobuf:"bytes,2,rep,name=pem_key_cert_pair" json:"pem_key_cert_pair,omitempty"`
	ForceClientAuth  bool                                                 `protobuf:"varint,3,opt,name=force_client_auth" json:"force_client_auth,omitempty"`
}

func (m *ArchiveServerConfig_SslCredentials) Reset()         { *m = ArchiveServerConfig_SslCredentials{} }
func (m *ArchiveServerConfig_SslCredentials) String() string { return proto.CompactTextString(m) }
func (*ArchiveServerConfig_SslCredentials) ProtoMessage()    {}

func (m *ArchiveServerConfig_SslCredentials) GetPemKeyCertPair() []*ArchiveServerConfig_SslCredentials_PemKeyCertPair {
	if m != nil {
		return m.PemKeyCertPair
	}
	return nil
}

type ArchiveServerConfig_SslCredentials_PemKeyCertPair struct {
	PrivateKeyPath string `protobuf:"bytes,1,opt,name=private_key_path" json:"private_key_path,omitempty"`
	CertChainPath  string `protobuf:"bytes,2,opt,name=cert_chain_path" json:"cert_chain_path,omitempty"`
}

func (m *ArchiveServerConfig_SslCredentials_PemKeyCertPair) Reset() {
	*m = ArchiveServerConfig_SslCredentials_PemKeyCertPair{}
}
func (m *ArchiveServerConfig_SslCredentials_PemKeyCertPair) String() string {
	return proto.CompactTextString(m)
}
func (*ArchiveServerConfig_SslCredentials_PemKeyCertPair) ProtoMessage() {}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConn

// Client API for Archive service

type ArchiveClient interface {
	Upload(ctx context.Context, opts ...grpc.CallOption) (Archive_UploadClient, error)
	Download(ctx context.Context, in *DownloadRequest, opts ...grpc.CallOption) (Archive_DownloadClient, error)
	Rename(ctx context.Context, in *RenameRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusResult, error)
	Existing(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error)
	ExistingAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error)
	Status(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	StatusAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	ImportResult(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem3.ImportMap, error)
	Import(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	ImportAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	// Flag API is not stable. Not intended for public usage.
	WithFlag(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error)
	WithFlagAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error)
	SetFlags(ctx context.Context, in *ChangeFlagsRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	UnsetFlags(ctx context.Context, in *ChangeFlagsRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
	ClearFlags(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error)
}

type archiveClient struct {
	cc *grpc.ClientConn
}

func NewArchiveClient(cc *grpc.ClientConn) ArchiveClient {
	return &archiveClient{cc}
}

func (c *archiveClient) Upload(ctx context.Context, opts ...grpc.CallOption) (Archive_UploadClient, error) {
	stream, err := grpc.NewClientStream(ctx, &_Archive_serviceDesc.Streams[0], c.cc, "/bacs.archive.Archive/Upload", opts...)
	if err != nil {
		return nil, err
	}
	x := &archiveUploadClient{stream}
	return x, nil
}

type Archive_UploadClient interface {
	Send(*Chunk) error
	CloseAndRecv() (*bacs_archive_problem4.StatusMap, error)
	grpc.ClientStream
}

type archiveUploadClient struct {
	grpc.ClientStream
}

func (x *archiveUploadClient) Send(m *Chunk) error {
	return x.ClientStream.SendMsg(m)
}

func (x *archiveUploadClient) CloseAndRecv() (*bacs_archive_problem4.StatusMap, error) {
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	m := new(bacs_archive_problem4.StatusMap)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *archiveClient) Download(ctx context.Context, in *DownloadRequest, opts ...grpc.CallOption) (Archive_DownloadClient, error) {
	stream, err := grpc.NewClientStream(ctx, &_Archive_serviceDesc.Streams[1], c.cc, "/bacs.archive.Archive/Download", opts...)
	if err != nil {
		return nil, err
	}
	x := &archiveDownloadClient{stream}
	if err := x.ClientStream.SendMsg(in); err != nil {
		return nil, err
	}
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	return x, nil
}

type Archive_DownloadClient interface {
	Recv() (*Chunk, error)
	grpc.ClientStream
}

type archiveDownloadClient struct {
	grpc.ClientStream
}

func (x *archiveDownloadClient) Recv() (*Chunk, error) {
	m := new(Chunk)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *archiveClient) Rename(ctx context.Context, in *RenameRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusResult, error) {
	out := new(bacs_archive_problem4.StatusResult)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/Rename", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) Existing(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error) {
	out := new(bacs_archive_problem1.IdSet)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/Existing", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) ExistingAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error) {
	out := new(bacs_archive_problem1.IdSet)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/ExistingAll", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) Status(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/Status", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) StatusAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/StatusAll", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) ImportResult(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem3.ImportMap, error) {
	out := new(bacs_archive_problem3.ImportMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/ImportResult", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) Import(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/Import", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) ImportAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/ImportAll", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) WithFlag(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error) {
	out := new(bacs_archive_problem1.IdSet)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/WithFlag", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) WithFlagAll(ctx context.Context, in *google_protobuf1.Empty, opts ...grpc.CallOption) (*bacs_archive_problem1.IdSet, error) {
	out := new(bacs_archive_problem1.IdSet)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/WithFlagAll", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) SetFlags(ctx context.Context, in *ChangeFlagsRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/SetFlags", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) UnsetFlags(ctx context.Context, in *ChangeFlagsRequest, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/UnsetFlags", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *archiveClient) ClearFlags(ctx context.Context, in *bacs_archive_problem1.IdSet, opts ...grpc.CallOption) (*bacs_archive_problem4.StatusMap, error) {
	out := new(bacs_archive_problem4.StatusMap)
	err := grpc.Invoke(ctx, "/bacs.archive.Archive/ClearFlags", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// Server API for Archive service

type ArchiveServer interface {
	Upload(Archive_UploadServer) error
	Download(*DownloadRequest, Archive_DownloadServer) error
	Rename(context.Context, *RenameRequest) (*bacs_archive_problem4.StatusResult, error)
	Existing(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem1.IdSet, error)
	ExistingAll(context.Context, *google_protobuf1.Empty) (*bacs_archive_problem1.IdSet, error)
	Status(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem4.StatusMap, error)
	StatusAll(context.Context, *google_protobuf1.Empty) (*bacs_archive_problem4.StatusMap, error)
	ImportResult(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem3.ImportMap, error)
	Import(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem4.StatusMap, error)
	ImportAll(context.Context, *google_protobuf1.Empty) (*bacs_archive_problem4.StatusMap, error)
	// Flag API is not stable. Not intended for public usage.
	WithFlag(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem1.IdSet, error)
	WithFlagAll(context.Context, *google_protobuf1.Empty) (*bacs_archive_problem1.IdSet, error)
	SetFlags(context.Context, *ChangeFlagsRequest) (*bacs_archive_problem4.StatusMap, error)
	UnsetFlags(context.Context, *ChangeFlagsRequest) (*bacs_archive_problem4.StatusMap, error)
	ClearFlags(context.Context, *bacs_archive_problem1.IdSet) (*bacs_archive_problem4.StatusMap, error)
}

func RegisterArchiveServer(s *grpc.Server, srv ArchiveServer) {
	s.RegisterService(&_Archive_serviceDesc, srv)
}

func _Archive_Upload_Handler(srv interface{}, stream grpc.ServerStream) error {
	return srv.(ArchiveServer).Upload(&archiveUploadServer{stream})
}

type Archive_UploadServer interface {
	SendAndClose(*bacs_archive_problem4.StatusMap) error
	Recv() (*Chunk, error)
	grpc.ServerStream
}

type archiveUploadServer struct {
	grpc.ServerStream
}

func (x *archiveUploadServer) SendAndClose(m *bacs_archive_problem4.StatusMap) error {
	return x.ServerStream.SendMsg(m)
}

func (x *archiveUploadServer) Recv() (*Chunk, error) {
	m := new(Chunk)
	if err := x.ServerStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func _Archive_Download_Handler(srv interface{}, stream grpc.ServerStream) error {
	m := new(DownloadRequest)
	if err := stream.RecvMsg(m); err != nil {
		return err
	}
	return srv.(ArchiveServer).Download(m, &archiveDownloadServer{stream})
}

type Archive_DownloadServer interface {
	Send(*Chunk) error
	grpc.ServerStream
}

type archiveDownloadServer struct {
	grpc.ServerStream
}

func (x *archiveDownloadServer) Send(m *Chunk) error {
	return x.ServerStream.SendMsg(m)
}

func _Archive_Rename_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(RenameRequest)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).Rename(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_Existing_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).Existing(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_ExistingAll_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(google_protobuf1.Empty)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).ExistingAll(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_Status_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).Status(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_StatusAll_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(google_protobuf1.Empty)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).StatusAll(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_ImportResult_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).ImportResult(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_Import_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).Import(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_ImportAll_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(google_protobuf1.Empty)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).ImportAll(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_WithFlag_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).WithFlag(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_WithFlagAll_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(google_protobuf1.Empty)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).WithFlagAll(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_SetFlags_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(ChangeFlagsRequest)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).SetFlags(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_UnsetFlags_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(ChangeFlagsRequest)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).UnsetFlags(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func _Archive_ClearFlags_Handler(srv interface{}, ctx context.Context, codec grpc.Codec, buf []byte) (interface{}, error) {
	in := new(bacs_archive_problem1.IdSet)
	if err := codec.Unmarshal(buf, in); err != nil {
		return nil, err
	}
	out, err := srv.(ArchiveServer).ClearFlags(ctx, in)
	if err != nil {
		return nil, err
	}
	return out, nil
}

var _Archive_serviceDesc = grpc.ServiceDesc{
	ServiceName: "bacs.archive.Archive",
	HandlerType: (*ArchiveServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "Rename",
			Handler:    _Archive_Rename_Handler,
		},
		{
			MethodName: "Existing",
			Handler:    _Archive_Existing_Handler,
		},
		{
			MethodName: "ExistingAll",
			Handler:    _Archive_ExistingAll_Handler,
		},
		{
			MethodName: "Status",
			Handler:    _Archive_Status_Handler,
		},
		{
			MethodName: "StatusAll",
			Handler:    _Archive_StatusAll_Handler,
		},
		{
			MethodName: "ImportResult",
			Handler:    _Archive_ImportResult_Handler,
		},
		{
			MethodName: "Import",
			Handler:    _Archive_Import_Handler,
		},
		{
			MethodName: "ImportAll",
			Handler:    _Archive_ImportAll_Handler,
		},
		{
			MethodName: "WithFlag",
			Handler:    _Archive_WithFlag_Handler,
		},
		{
			MethodName: "WithFlagAll",
			Handler:    _Archive_WithFlagAll_Handler,
		},
		{
			MethodName: "SetFlags",
			Handler:    _Archive_SetFlags_Handler,
		},
		{
			MethodName: "UnsetFlags",
			Handler:    _Archive_UnsetFlags_Handler,
		},
		{
			MethodName: "ClearFlags",
			Handler:    _Archive_ClearFlags_Handler,
		},
	},
	Streams: []grpc.StreamDesc{
		{
			StreamName:    "Upload",
			Handler:       _Archive_Upload_Handler,
			ClientStreams: true,
		},
		{
			StreamName:    "Download",
			Handler:       _Archive_Download_Handler,
			ServerStreams: true,
		},
	},
}
