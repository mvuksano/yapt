#include "event.h"

#include <arpa/inet.h>
#include <glog/logging.h>

#include <chrono>
#include <iomanip>
#include <iostream>

#include "context.h"
#include "eventbase.h"
#include "icmp.h"

#define MAX_PACKET_SIZE 64

template <typename T>
struct event *Event<T>::get() {
  return evt;
}

template <>
Event<WriteEv>::Event(EventBase &evb) = delete;

template <>
Event<WriteEv>::Event(EventBase &evb, int fd) {
  this->evt = event_new(
      evb.get(), static_cast<evutil_socket_t>(fd), EV_WRITE,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        auto dest_ip = Context::ips[0];
        Context::ips.erase(Context::ips.begin());
        if (inet_pton(AF_INET, dest_ip.c_str(), &dest_addr.sin_addr) != 1) {
          LOG(FATAL)
              << "Failed to convert target ip address to network presentation.";
        }

        char packet[MAX_PACKET_SIZE];
        size_t packet_size = MAX_PACKET_SIZE;
        create_ping_request(packet, packet_size, 1);

        VLOG(6) << "Sending ICMP ping packet to " << dest_ip.c_str()
                << " using fd " << socket_fd;
        Context::expected.push_back(dest_ip.c_str());
        auto start_time = std::chrono::high_resolution_clock::now();
        Context::time_map[dest_ip.c_str()] = start_time;
        auto ret = sendto(socket_fd, packet, packet_size, 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret < 0) {
          LOG(FATAL) << "Failed to send ping request. sendto syscall returned"
                     << ret << ", errno is: " << errno;
        }

        if (not Context::ips.empty()) {
          VLOG(6)
              << "There are more ip addresses to process. Activating event.";
          event_add((struct event *)arg, nullptr);
        }
      },
      event_self_cbarg());
  event_add(evt, nullptr);
  VLOG(6) << "Added write event for socket " << fd;
}

template <>
Event<ReadEv>::Event(EventBase &evb) = delete;

template <>
Event<ReadEv>::Event(EventBase &evb, int fd) {
  struct timeval five_seconds = {5, 0};
  this->evt = event_new(
      evb.get(), static_cast<evutil_socket_t>(fd), EV_TIMEOUT | EV_READ,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        VLOG(6) << "Read callback triggered due to " << what;
        if (what == EV_TIMEOUT) {
          return;
        }

        struct sockaddr_in src_addr;
        socklen_t src_len = sizeof(src_addr);
        unsigned char recv_buf[MAX_PACKET_SIZE];
        int recv_len = recvfrom(socket_fd, recv_buf, MAX_PACKET_SIZE, 0,
                                (struct sockaddr *)&src_addr, &src_len);
        if (recv_len < 0) {
          LOG(FATAL) << "Failed to read data from socket " << socket_fd;
        }

        auto end_time = std::chrono::high_resolution_clock::now();

        auto src_addr_as_str = inet_ntoa(src_addr.sin_addr);
        auto start_time = Context::time_map.find(src_addr_as_str)->second;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);
        VLOG(6) << "Received " << recv_len << " bytes from " << src_addr_as_str
                << " on socket " << socket_fd;
        LOG(INFO) << std::left << std::setw(10) << src_addr_as_str << "\t"
                  << duration;
        std::stringstream message;
        for (auto i = 0; i < MAX_PACKET_SIZE; i++) {
          message << std::hex << std::setfill('0') << std::setw(2)
                  << (int)recv_buf[i] << " ";
        }
        VLOG(7) << message.str();

        for (auto it = Context::expected.begin(); it != Context::expected.end();
             it++) {
          if (*it == src_addr_as_str) {
            Context::expected.erase(it);
            break;
          }
        }
        if (not Context::expected.empty()) {
          struct timeval five_seconds = {5, 0};
          event_add((struct event *)arg, &five_seconds);
        } else {
          VLOG(6) << "Not expecting any more responses and thus not activating "
                     "event.";
        }
      },
      event_self_cbarg());
  event_add(evt, &five_seconds);
  VLOG(6) << "Added read event for socket " << fd;
}

template <>
Event<Periodic>::Event(EventBase &evb) {
  struct timeval one_sec = {1, 0};
  auto ev = event_new(
      evb.get(), -1, 0,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        VLOG(6) << "Timer callback running.";
        struct timeval one_sec = {1, 0};
        event_add((struct event *)arg, &one_sec);

        VLOG(6) << "Copying destination IPs to ping.";
        for (auto &ip : Context::expected) {
          LOG(INFO) << std::left << std::setw(10) << ip << "\t"
                    << ">1s";
        }
        Context::expected = {};
        Context::ips = Context::ipsToPing;

        event_add(Context::writeEvent->get(), nullptr);
        event_add(Context::readEvent->get(), nullptr);
      },
      event_self_cbarg());
  event_add(ev, &one_sec);
}

template <>
Event<Periodic>::Event(EventBase &evb, int fd) = delete;

template <typename T>
Event<T>::~Event() {
  VLOG(6) << "Clearning up event";
  event_free(evt);
}