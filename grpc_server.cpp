#include "grpc_server.h"

#include <glog/logging.h>
#include <jemalloc/jemalloc.h>

#include <memory>
#include <thread>

#include "protos/service.grpc.pb.h"

class ProfileServiceImpl final : public Profile::Service {
  ::grpc::Status EnableProfiling(::grpc::ServerContext* context,
                                 const ::EnableProfilingRequest* request,
                                 ::EnableProfilingReply* response) {
    bool oldvalue;
    size_t oldlen;
    mallctl("prof.active", &oldvalue, &oldlen, nullptr, 0);
    LOG(INFO) << "prof.active " << oldvalue;
    bool newvalue;
    if (oldvalue == 0) {
      newvalue = true;
    } else {
      newvalue = false;
    }
    LOG(INFO) << "prof.active setting new value " << newvalue;
    mallctl("prof.active", &oldvalue, &oldlen, &newvalue, sizeof(newvalue));
    return ::grpc::Status::OK;
  }
};

GrpcServer::GrpcServer(std::string addressAndPort)
    : server_address_(addressAndPort) {}

void GrpcServer::startInThread() {
  serverThread_ = std::thread([this]() {
    ProfileServiceImpl profileService;
    ServerBuilder builder;
    builder.AddListeningPort(server_address_,
                             grpc::InsecureServerCredentials());
    builder.RegisterService(&profileService);
    server_ = builder.BuildAndStart();
    LOG(INFO) << "Server listening on " << server_address_ << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server_->Wait();
  });
}

void GrpcServer::stop() {
  server_->Shutdown();
  serverThread_.join();
}

GrpcServer::~GrpcServer() { LOG(INFO) << "GrpcServer dtor"; }