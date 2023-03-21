#ifndef UTILS_HPP
#define UTILS_HPP

#include <arpa/inet.h>
#include <glog/logging.h>

#include <boost/scope_exit.hpp>
#include <fstream>
#include <vector>

using IPAddr = uint32_t;

std::vector<IPAddr> getIpsToPingFromFile(const std::string fileName) {
  std::ifstream inputFile;
  std::string line;
  std::vector<IPAddr> ipAddrs;

  inputFile.open(fileName.c_str(), std::ios::in);

  if (inputFile.good()) {
    BOOST_SCOPE_EXIT(&inputFile) { inputFile.close(); }
    BOOST_SCOPE_EXIT_END

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
#endif