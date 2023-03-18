#include "application.h"

#include <event2/event.h>
#include <glog/logging.h>

#include <iostream>
#include <sstream>
#include <vector>

#include "context.h"
#include "eventbase.h"

App::App(std::vector<IpAddr>&& ipList) {
  Context::ipsToPing = std::move(ipList);

  for (auto it = Context::ipsToPing.begin(); it != Context::ipsToPing.end();
       ++it) {
    Context::ips.push_back(*it);
  }

  auto sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    LOG(FATAL) << "Could not create RAW socket: " << errno;
  }
  VLOG(6) << "Created socket. Socket fd is " << sockfd;

  Context::readEvent = new Event<ReadEv>(evb, sockfd);
  Context::writeEvent = new Event<WriteEv>(evb, sockfd);
  Context::periodicTimerEvent = new Event<Periodic>(evb);
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
