#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include <grpcpp/server_builder.h>

#include <thread>

using grpc::ServerBuilder;

class GrpcServer {
 public:
  GrpcServer(std::string addressAndPort);
  void startInThread();
  ~GrpcServer();
  void stop();

 private:
  std::thread serverThread_;
  std::unique_ptr<grpc::Server> server_;
  std::string server_address_;
};

#endif