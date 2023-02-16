#ifndef APPLICATION_H
#define APPLICATION_H

#include <event2/event.h>

#include <memory>
#include <thread>
#include <vector>

#include "event.h"
#include "eventbase.h"

class App {
 public:
  App(std::string);
  void run();

 private:
  EventBase evb;
  std::thread evb_thread;
};

#endif