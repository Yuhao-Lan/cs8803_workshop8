// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: master-worker.proto

#include "master-worker.pb.h"
#include "master-worker.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace masterworker {

static const char* Worker_method_names[] = {
  "/masterworker.Worker/StartMapper",
  "/masterworker.Worker/StartReducer",
  "/masterworker.Worker/Ping",
};

std::unique_ptr< Worker::Stub> Worker::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Worker::Stub> stub(new Worker::Stub(channel));
  return stub;
}

Worker::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_StartMapper_(Worker_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_StartReducer_(Worker_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Ping_(Worker_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Worker::Stub::StartMapper(::grpc::ClientContext* context, const ::masterworker::Filename& request, ::masterworker::Filename* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_StartMapper_, context, request, response);
}

void Worker::Stub::experimental_async::StartMapper(::grpc::ClientContext* context, const ::masterworker::Filename* request, ::masterworker::Filename* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_StartMapper_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::masterworker::Filename>* Worker::Stub::AsyncStartMapperRaw(::grpc::ClientContext* context, const ::masterworker::Filename& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::Filename>::Create(channel_.get(), cq, rpcmethod_StartMapper_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::masterworker::Filename>* Worker::Stub::PrepareAsyncStartMapperRaw(::grpc::ClientContext* context, const ::masterworker::Filename& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::Filename>::Create(channel_.get(), cq, rpcmethod_StartMapper_, context, request, false);
}

::grpc::Status Worker::Stub::StartReducer(::grpc::ClientContext* context, const ::masterworker::Filenames& request, ::masterworker::Filename* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_StartReducer_, context, request, response);
}

void Worker::Stub::experimental_async::StartReducer(::grpc::ClientContext* context, const ::masterworker::Filenames* request, ::masterworker::Filename* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_StartReducer_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::masterworker::Filename>* Worker::Stub::AsyncStartReducerRaw(::grpc::ClientContext* context, const ::masterworker::Filenames& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::Filename>::Create(channel_.get(), cq, rpcmethod_StartReducer_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::masterworker::Filename>* Worker::Stub::PrepareAsyncStartReducerRaw(::grpc::ClientContext* context, const ::masterworker::Filenames& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::Filename>::Create(channel_.get(), cq, rpcmethod_StartReducer_, context, request, false);
}

::grpc::Status Worker::Stub::Ping(::grpc::ClientContext* context, const ::masterworker::PingData& request, ::masterworker::PingData* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Ping_, context, request, response);
}

void Worker::Stub::experimental_async::Ping(::grpc::ClientContext* context, const ::masterworker::PingData* request, ::masterworker::PingData* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Ping_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::masterworker::PingData>* Worker::Stub::AsyncPingRaw(::grpc::ClientContext* context, const ::masterworker::PingData& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::PingData>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::masterworker::PingData>* Worker::Stub::PrepareAsyncPingRaw(::grpc::ClientContext* context, const ::masterworker::PingData& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::masterworker::PingData>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, false);
}

Worker::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Worker_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Worker::Service, ::masterworker::Filename, ::masterworker::Filename>(
          std::mem_fn(&Worker::Service::StartMapper), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Worker_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Worker::Service, ::masterworker::Filenames, ::masterworker::Filename>(
          std::mem_fn(&Worker::Service::StartReducer), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Worker_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Worker::Service, ::masterworker::PingData, ::masterworker::PingData>(
          std::mem_fn(&Worker::Service::Ping), this)));
}

Worker::Service::~Service() {
}

::grpc::Status Worker::Service::StartMapper(::grpc::ServerContext* context, const ::masterworker::Filename* request, ::masterworker::Filename* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Worker::Service::StartReducer(::grpc::ServerContext* context, const ::masterworker::Filenames* request, ::masterworker::Filename* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Worker::Service::Ping(::grpc::ServerContext* context, const ::masterworker::PingData* request, ::masterworker::PingData* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace masterworker

