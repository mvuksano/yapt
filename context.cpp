#include "context.h"

#include "event.h"

Context::flatmap<IpAddr, std::chrono::time_point<std::chrono::system_clock>>
    Context::time_map{};

std::vector<IpAddr> Context::ips{};
std::vector<IpAddr> Context::ipsToPing{};

std::set<IpAddr> Context::expected{};

Event<ReadEv> *Context::readEvent;
Event<WriteEv> *Context::writeEvent;
Event<Periodic> *Context::periodicTimerEvent;
