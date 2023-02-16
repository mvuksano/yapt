#include <netinet/ip_icmp.h>

#define ICMP_ECHO_REQUEST 8
#define MAX_PACKET_SIZE 64
#define TIMEOUT 3

uint16_t checksum(const uint16_t *b, size_t len) {
  uint32_t sum = 0;
  uint16_t result;

  size_t octets = len / 2;
  bool is_even = len % 2;

  for (auto i = 0; i < octets; i++) {
    sum += *(b + i);
  }

  if (is_even) {
    sum += *(reinterpret_cast<const uint8_t *>(b));
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  result = static_cast<uint16_t>(~sum);

  return result;
}

size_t create_ping_request(char *packet, size_t packet_size, uint32_t seq) {
  struct icmphdr *icmp_header;
  char *data;

  icmp_header = reinterpret_cast<struct icmphdr *>(packet);
  icmp_header->type = ICMP_ECHO_REQUEST;
  icmp_header->code = 0;
  icmp_header->un.echo.id = getpid();
  icmp_header->un.echo.sequence = seq;
  icmp_header->checksum = 0;

  // Fill in data part of the packet
  data = packet + sizeof(struct icmphdr);
  memset(data, 0xA5, packet_size - sizeof(struct icmphdr));

  icmp_header->checksum =
      checksum(reinterpret_cast<const uint16_t *>(packet), packet_size);

  return packet_size;
}