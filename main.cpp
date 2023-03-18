#include <arpa/inet.h>
#include <event2/thread.h>
#include <folly/ScopeGuard.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "application.h"

DEFINE_string(ips, "", "comma separated list of ips to ping.");

DEFINE_string(ips_file, "", "File which contains list of IPs to ping.");

using IPAddr = uint32_t;

std::vector<IPAddr> getIpsToPingFromFile(const std::string fileName) {
  std::ifstream inputFile;
  std::string line;
  std::vector<IPAddr> ipAddrs;

  inputFile.open(fileName.c_str(), std::ios::in);

  if (inputFile.good()) {
    auto guard = folly::makeGuard([&] { inputFile.close(); });

    while (true) {
      if (!getline(inputFile, line)) {
        break;
      }
      struct in_addr ipAddr;
      if (inet_pton(AF_INET, line.c_str(), &ipAddr) <= 0) {
        LOG(FATAL) << "Couldn't convert " << line << " to an IP address";
      }
      ipAddrs.push_back(ipAddr.s_addr);
    };
  }

  return ipAddrs;
}

std::vector<IPAddr> parseIPsArgs(const std::string ips) {
  std::vector<IPAddr> ipAddrs;
  std::istringstream iss(ips);

  std::string ip;
  while (std::getline(iss, ip, ',')) {
    IPAddr ipAddr;

    if (inet_pton(AF_INET, ip.c_str(), &ipAddr) <= 0) {
      LOG(FATAL) << "Couldn't convert " << ip << " to an IP address";
    }

    ipAddrs.push_back(std::move(ipAddr));
    ip = "";
  }

  return ipAddrs;
}

std::string ipsAsString(const std::vector<IPAddr> ipAddrs) {
  std::string ips = "";
  for (auto it = ipAddrs.cbegin(); it != ipAddrs.end(); *it++) {
    struct in_addr ip_address;
    ip_address.s_addr = *it;
    auto ipAsStr = inet_ntoa(ip_address);
    ips.append(ipAsStr).append(",");
  }

  ips.resize(ips.size() - 1);
  return ips;
}

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (evthread_use_pthreads() != 0) {
    LOG(FATAL) << "libevent not built for use in a multithreaded environment.";
  }

  std::vector<IPAddr> ipsToPing{};
  if (FLAGS_ips_file != "") {
    ipsToPing = getIpsToPingFromFile(FLAGS_ips_file);
  }

  if (FLAGS_ips != "") {
    ipsToPing = parseIPsArgs(FLAGS_ips);
  }

  LOG_IF(FATAL, ipsToPing.empty()) << "No IPs to ping.";

  std::sort(ipsToPing.begin(), ipsToPing.end(),
            [](auto a, auto b) { return a < b; });

  auto tmp = ipsAsString(ipsToPing);
  VLOG(6) << "Pinging the following IPs: " << tmp;

  // TODO: Pass IPs to APP w/o converting them to string first.
  App app{std::move(ipsToPing)};
  app.run();

  return EXIT_SUCCESS;
}
