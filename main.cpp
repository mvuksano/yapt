#include <event2/thread.h>
#include <folly/ScopeGuard.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "application.h"

DEFINE_string(ips, "", "comma separated list of ips to ping.");

DEFINE_string(ips_file, "", "File which contains list of IPs to ping.");

std::string getIpsToPingFromFile(const std::string fileName) {
  std::ifstream inputFile;
  std::string line;
  std::string ips;

  inputFile.open(fileName.c_str(), std::ios::in);

  if (inputFile.good()) {
    auto guard = folly::makeGuard([&] { inputFile.close(); });

    while (true) {
      if (!getline(inputFile, line)) {
        break;
      }
      ips.append(line).append(",");
    };

    ips.resize(ips.size() - 2);
  }

  return ips;
}

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (evthread_use_pthreads() != 0) {
    LOG(FATAL) << "libevent not built for use in a multithreaded environment.";
  }

  if (FLAGS_ips_file != "") {
    FLAGS_ips = getIpsToPingFromFile(FLAGS_ips_file);
  }

  LOG_IF(FATAL, FLAGS_ips == "") << "No IPs to ping.";

  App app{FLAGS_ips};
  app.run();

  return EXIT_SUCCESS;
}
