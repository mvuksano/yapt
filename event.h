#ifndef EVENT_H
#define EVENT_H

#include <event2/event.h>

#include "eventbase.h"

struct ReadEv {};
struct WriteEv {};
struct Periodic {};

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <IsAnyOf<ReadEv, WriteEv, Periodic> T>
class Event {
 public:
  Event(EventBase &);
  Event(EventBase &, int);
  ~Event();
  struct event *get();
  bool enabled();

 private:
  struct event *evt;
};

#endif