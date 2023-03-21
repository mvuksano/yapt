#ifndef CONTEXT_H
#define CONTEXT_H

#include <arpa/inet.h>

#include <boost/unordered/unordered_flat_map.hpp>
#include <chrono>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "event.h"

using IpAddr = uint32_t;

class Context {
  template <typename K, typename V>
  using flatmap = boost::unordered_flat_map<K, V>;

 public:
  static flatmap<IpAddr, std::chrono::time_point<std::chrono::system_clock>>
      time_map;
  static std::vector<IpAddr> ips;
  static std::vector<IpAddr> ipsToPing;
  static std::set<IpAddr> expected;

  static Event<ReadEv> *readEvent;
  static Event<WriteEv> *writeEvent;
  static Event<Periodic> *periodicTimerEvent;
};

#endif