#ifndef EVENT_H
#define EVENT_H

#include <event2/event.h>

#include "eventbase.h"

struct ReadEv {};
struct WriteEv {};
struct Periodic {};

template <typename T>
class Event {
 public:
  Event(EventBase &);
  Event(EventBase &, int);
  ~Event();
  struct event * get();

 private:
  struct event *evt;
};

#endif