#include "event.h"

#include <arpa/inet.h>
#include <glog/logging.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "context.h"
#include "eventbase.h"
#include "icmp.h"

#define MAX_PACKET_SIZE 64

template <IsAnyOf<ReadEv, WriteEv, Periodic> T>
struct event *Event<T>::get() {
  return evt;
}

template <IsAnyOf<ReadEv, WriteEv, Periodic> T>
bool Event<T>::enabled() {
  return event_pending(this->evt, EV_READ | EV_WRITE | EV_SIGNAL | EV_TIMEOUT,
                       nullptr);
}

template <>
Event<WriteEv>::Event(EventBase &evb, std::fstream &logFile) = delete;

template <>
Event<WriteEv>::Event(EventBase &evb, int fd, std::fstream &logFile)
    : logFile(logFile) {
  this->evt = event_new(
      evb.get(), static_cast<evutil_socket_t>(fd), EV_WRITE,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        VLOG(6) << "Write callback triggered due to " << what;
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        auto dest_ip = Context::ips[0];
        dest_addr.sin_addr.s_addr = dest_ip;
        Context::ips.erase(Context::ips.begin());
        auto destIpStr = inet_ntoa(dest_addr.sin_addr);

        char packet[MAX_PACKET_SIZE];
        size_t packet_size = MAX_PACKET_SIZE;
        create_ping_request(packet, packet_size, 1);

        VLOG(6) << "Sending ICMP ping packet to " << destIpStr << " using fd "
                << socket_fd;
        auto firstLower = std::lower_bound(Context::expected.cbegin(),
                                           Context::expected.cend(), dest_ip);
        Context::expected.insert(firstLower, dest_ip);
        auto start_time = std::chrono::high_resolution_clock::now();
        Context::time_map[dest_ip] = start_time;
        auto ret = sendto(socket_fd, packet, packet_size, 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret < 0) {
          LOG(FATAL) << "Failed to send ping request. sendto syscall returned "
                     << ret << ", errno is: " << errno;
        }

        if (not Context::ips.empty()) {
          VLOG(6) << "There are more ip addresses to process. Activating write "
                     "event.";
          event_add((struct event *)arg, nullptr);
        } else {
          VLOG(6) << "There are no more ip addresses to process. Not "
                     "activating write event.";
        }
      },
      event_self_cbarg());
  event_add(evt, nullptr);
  VLOG(6) << "Added write event for socket " << fd;
}

template <>
Event<ReadEv>::Event(EventBase &evb, std::fstream &logFile) = delete;

template <>
Event<ReadEv>::Event(EventBase &evb, int fd, std::fstream &logFile)
    : logFile(logFile) {
  this->evt = event_new(
      evb.get(), static_cast<evutil_socket_t>(fd),
      EV_TIMEOUT | EV_READ | EV_PERSIST,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        VLOG(6) << "Read callback triggered due to " << what;

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
        VLOG(6) << "Received packet from: " << src_addr_as_str;
        if (not std::binary_search(Context::expected.cbegin(),
                                   Context::expected.cend(),
                                   src_addr.sin_addr.s_addr)) {
          VLOG(3)
              << "Packet that was received was not expected from this host ("
              << src_addr_as_str << ")";
          return;
        }
        if (std::all_of(Context::ipsToPing.cbegin(), Context::ipsToPing.cend(),
                        [&src_addr](auto ip) {
                          return ip != src_addr.sin_addr.s_addr;
                        })) {
          VLOG(3)
              << "Received ICMP packet from a host we are not interested in.";
          return;
        }
        auto start_time =
            Context::time_map.find(src_addr.sin_addr.s_addr)->second;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);
        VLOG(6) << "Received " << recv_len << " bytes from " << src_addr_as_str
                << " on socket " << socket_fd;
        LOG(INFO) << std::left << std::setw(10) << src_addr_as_str << "\t"
                  << duration.count() << " ms";
        auto dur = duration.count();
        ((std::fstream *)arg)
            ->write(reinterpret_cast<char *>(&dur), sizeof(dur));
        std::stringstream message;
        for (auto i = 0; i < MAX_PACKET_SIZE; i++) {
          message << std::hex << std::setfill('0') << std::setw(2)
                  << (int)recv_buf[i] << " ";
        }
        VLOG(7) << message.str();

        auto element = std::lower_bound(Context::expected.cbegin(),
                                        Context::expected.cend(),
                                        src_addr.sin_addr.s_addr);
        struct sockaddr_in ip_addr;
        ip_addr.sin_family = AF_INET;
        ip_addr.sin_addr.s_addr = *element;
        auto ip_as_str = inet_ntoa(ip_addr.sin_addr);
        VLOG(7) << "Found lower_bound element " << *ip_as_str
                << " while processing packet received from: "
                << src_addr_as_str;
        if (element != Context::expected.cend()) {
          Context::expected.erase(element);
        } else {
        }
        VLOG(6) << "Reactivating read event.";
        return;
      },
      &logFile);
  event_add(evt, nullptr);
  VLOG(6) << "Added read event for socket " << fd;
}

template <>
Event<Periodic>::Event(EventBase &evb, std::fstream &logFile)
    : logFile(logFile) {
  struct timeval one_sec = {1, 0};
  auto ev = event_new(
      evb.get(), -1, 0,
      [](evutil_socket_t socket_fd, short what, void *arg) {
        VLOG(6) << "Timer callback running.";
        struct timeval one_sec = {1, 0};
        event_add((struct event *)arg, &one_sec);

        if (Context::writeEvent->enabled()) {
          VLOG(6) << "There are more IPs to query, not doing anything for now.";
          return;
        }

        VLOG(6) << "Copying destination IPs to ping.";
        for (auto &ip : Context::expected) {
          struct sockaddr_in dest_addr;
          dest_addr.sin_family = AF_INET;
          dest_addr.sin_addr.s_addr = ip;
          auto ip_as_str = inet_ntoa(dest_addr.sin_addr);
          LOG(INFO) << std::left << std::setw(10) << ip_as_str << "\t"
                    << "1500 ms";
        }
        Context::expected = {};
        Context::ips = Context::ipsToPing;
        Context::time_map.clear();

        event_add(Context::writeEvent->get(), nullptr);
        event_add(Context::readEvent->get(), nullptr);
      },
      event_self_cbarg());
  event_add(ev, &one_sec);
}

template <>
Event<Periodic>::Event(EventBase &evb, int fd, std::fstream &logFile) = delete;
