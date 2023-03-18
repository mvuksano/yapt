#ifndef APPLICATION_H
#define APPLICATION_H

#include <arpa/inet.h>
#include <event2/event.h>

#include <memory>
#include <thread>
#include <vector>

#include "event.h"
#include "eventbase.h"

class App {
 public:
  using IpAddr = uint32_t;

  App(std::vector<IpAddr>&& ipList);
  void run();

 private:
  EventBase evb;
  std::thread evb_thread;
};

#endif