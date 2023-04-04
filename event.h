#ifndef EVENT_H
#define EVENT_H

#include <event2/event.h>

#include <iostream>

#include "eventbase.h"

struct ReadEv {};
struct WriteEv {};
struct Periodic {};

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <IsAnyOf<ReadEv, WriteEv, Periodic> T>
class Event {
 public:
  Event(EventBase &, std::fstream &logFile);
  Event(EventBase &, int, std::fstream &logFile);
  ~Event();
  struct event *get();
  bool enabled();

 private:
  struct event *evt;
  std::fstream &logFile;
};

#endif