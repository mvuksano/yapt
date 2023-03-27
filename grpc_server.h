#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include <thread>
#include <grpcpp/server_builder.h>

using grpc::ServerBuilder;

class GrpcServer {
public:
    GrpcServer();

private:
    std::thread serverThread_;
};

#endif