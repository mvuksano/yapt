#ifndef APPLICATION_H
#define APPLICATION_H

#include <arpa/inet.h>
#include <event2/event.h>

#include <fstream>
#include <memory>
#include <thread>
#include <vector>

#include "event.h"
#include "eventbase.h"

class App {
 public:
  using IpAddr = uint32_t;

  static inline std::vector<App*> apps;

  App(std::vector<IpAddr>&& ipList, std::string outFile);
  void run();
  void stop();

 private:
  EventBase evb;
  std::thread evb_thread;
  std::fstream outFile;
};

#endif