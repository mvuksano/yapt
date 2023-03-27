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

GrpcServer::GrpcServer() {
  serverThread_ = std::thread([]() {
    std::string server_address = "localhost:50555";
    ProfileServiceImpl profileService;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&profileService);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "Server listening on "
              << "localhost:50555" << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
  });
}