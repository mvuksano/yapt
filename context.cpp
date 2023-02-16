#include "context.h"

#include "event.h"

Context::flatmap<std::string,
                 std::chrono::time_point<std::chrono::system_clock>>
    Context::time_map{};

std::vector<std::string> Context::ips{};
std::vector<std::string> Context::ipsToPing{};

std::vector<std::string> Context::expected{};

Event<ReadEv> *Context::readEvent;
Event<WriteEv> *Context::writeEvent;
Event<Periodic> *Context::periodicTimerEvent;