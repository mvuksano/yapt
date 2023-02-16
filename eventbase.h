#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <chrono>
#include <memory>
#include <unordered_map>

struct EventBaseDeleter {
  void operator()(struct event_base *);
};

class EventBase {
 public:
  EventBase();
  EventBase(EventBase &) = delete;
  ~EventBase() = default;
  void loop();
  struct event_base *get();

 private:
  std::shared_ptr<struct event_base> evb;
};

#endif