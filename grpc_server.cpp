#include <memory>
#include <glog/logging.h>
#include <thread>

#include "grpc_server.h"

#include "protos/service.grpc.pb.h"

class ProfileServiceImpl final: public Profile::Service  {
    ::grpc::Status EnableProfiling(::grpc::ServerContext* context, const ::EnableProfilingRequest* request, ::EnableProfilingReply* response) {
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
        LOG(INFO) << "Server listening on " << "localhost:50555" << std::endl;

        // Wait for the server to shutdown. Note that some other thread must be
        // responsible for shutting down the server for this call to ever return.
        server->Wait(); 
    });
}