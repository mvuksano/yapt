#ifndef CONTEXT_H
#define CONTEXT_H

#include <boost/unordered/unordered_flat_map.hpp>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "event.h"

class Context {
  template <typename K, typename V>
  using flatmap = boost::unordered_flat_map<K, V>;

 public:
  static flatmap<std::string,
                 std::chrono::time_point<std::chrono::system_clock>>
      time_map;
  static std::vector<std::string> ips;
  static std::vector<std::string> ipsToPing;
  static std::vector<std::string> expected;

  static Event<ReadEv> *readEvent;
  static Event<WriteEv> *writeEvent;
  static Event<Periodic> *periodicTimerEvent;
};

#endif