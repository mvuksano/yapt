#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include <grpcpp/server_builder.h>

#include <thread>

using grpc::ServerBuilder;

class GrpcServer {
 public:
  GrpcServer();

 private:
  std::thread serverThread_;
};

#endif