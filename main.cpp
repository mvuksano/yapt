#include <event2/thread.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "application.h"

DEFINE_string(
    ips,
    "8.8.8.8,8.8.4.4,1.1.1.1,1.0.0.1,208.67.222.222,208.67.220.220,9.9.9.9,149."
    "112.112.112,8.26.56.26,8.20.247.20,209.244.0.3,209.244.0.4,64.6.64.6,64.6."
    "65.6,77.88.8.8,77.88.8.1,199.85.126.10,199.85.127.10",
    "comma separated list of ips to ping.");

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (evthread_use_pthreads() != 0) {
    LOG(FATAL) << "libevent not built for use in a multithreaded environment.";
  }

  App app{FLAGS_ips};
  app.run();

  return EXIT_SUCCESS;
}
