#include "eventbase.h"

#include <event2/event.h>
#include <glog/logging.h>

void EventBaseDeleter::operator()(struct event_base *evb) {
  VLOG(6) << "Freeing up event base " << evb;
  event_base_free(evb);
}

EventBase::EventBase() {
  struct event_base *e = event_base_new();
  VLOG(6) << "Event base created at " << e;
  this->evb = std::shared_ptr<struct event_base>(e, EventBaseDeleter());
}

void EventBase::loop() {
  VLOG(6) << "Evb address in loop method: " << this->evb.get();
  event_base_loop(evb.get(), 0);
}

void EventBase::terminate() {
  VLOG(6) << "Terminating event base " << evb;
  event_base_loopbreak(evb.get());
}

struct event_base *EventBase::get() { return evb.get(); }
