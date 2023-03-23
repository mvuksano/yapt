#include <event2/event.h>
#include <event2/thread.h>
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
#include "build_options.h"
#include "utils.hpp"


DEFINE_string(ips, "", "comma separated list of ips to ping.");

DEFINE_string(ips_file, "", "File which contains list of IPs to ping.");

void log_to_glog(int severity, const char* msg) { VLOG(10) << msg; }

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (evthread_use_pthreads() != 0) {
    LOG(FATAL) << "libevent not built for use in a multithreaded environment.";
  }
  if constexpr(EVENTLIB_DEBUG_ENABLE) {
    event_set_log_callback(log_to_glog);
    event_enable_debug_logging(EVENT_DBG_ALL);
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
