#include "application.h"

#include <event2/event.h>
#include <glog/logging.h>

#include <boost/scope_exit.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "context.h"
#include "eventbase.h"

App::App(std::vector<IpAddr>&& ipList, std::string where) {
  LOG(INFO) << "App ctor";
  CHECK(where != "") << "Output file must be specified";
  Context::ipsToPing = std::move(ipList);

  outFile.open(where, std::fstream::out);
  CHECK(outFile.is_open()) << "output file must be opened.";

  for (auto it = Context::ipsToPing.begin(); it != Context::ipsToPing.end();
       ++it) {
    Context::ips.push_back(*it);
  }

  auto sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    LOG(FATAL) << "Could not create RAW socket: " << errno;
  }
  VLOG(6) << "Created socket. Socket fd is " << sockfd;

  Context::readEvent = new Event<ReadEv>(evb, sockfd, outFile);
  Context::writeEvent = new Event<WriteEv>(evb, sockfd, outFile);
  Context::periodicTimerEvent = new Event<Periodic>(evb, outFile);

  apps.push_back(this);
}

void App::run() {
  evb_thread = std::thread([this]() {
    VLOG(6) << "Event base (evb) thread is runnung."
            << "Evb address (memory location) is: " << evb.get();
    evb.loop();
    VLOG(6) << "Event base (evb) thread completed.";
  });
  evb_thread.join();
  VLOG(6) << "Event base (evb) thread joined with main thread.";
}

void App::stop() { evb.terminate(); }
