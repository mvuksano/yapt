#include <event2/event.h>
#include <event2/thread.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <signal.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "application.h"
#include "build_options.h"
#include "grpc_server.h"
#include "utils.hpp"

DEFINE_string(ips, "", "comma separated list of ips to ping.");

DEFINE_string(ips_file, "", "File which contains list of IPs to ping.");

DEFINE_string(out_file, "", "File where to output measurements.");

void log_to_glog(int severity, const char* msg) { VLOG(10) << msg; }

void signalHandler(int sig) {
  VLOG(6) << "Received signal " << sig;
  for (auto app : App::apps) {
    app->stop();
  }
}

void installSignalHandlers() {
  struct sigaction sa;
  sa.sa_handler = signalHandler;
  sa.sa_flags = 0;
  for (auto signal : {SIGINT, SIGQUIT, SIGTERM}) {
    sigaction(signal, &sa, nullptr);
  }
}

int main(int argc, char** argv) {
  installSignalHandlers();
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (evthread_use_pthreads() != 0) {
    LOG(FATAL) << "libevent not built for use in a multithreaded environment.";
  }
  if constexpr (EVENTLIB_DEBUG_ENABLE) {
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

  GrpcServer server("localhost:50555");
  server.startInThread();

  App app{std::move(ipsToPing), FLAGS_out_file};
  app.run();

  server.stop();

  return EXIT_SUCCESS;
}
