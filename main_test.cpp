#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils.hpp"

TEST(FOO, BAR) { ASSERT_EQ(2, 2); }

TEST(PARSING_IPS, PARSE_IPS_ARG) {
  std::string ips = "1.1.1.1,8.8.8.8";
  auto ipsVec = parseIPsArgs(ips);
  auto expectedIpsVec =
      std::vector<IPAddr>{IPAddr{0x08080808}, IPAddr{0x01010101}};
  auto m = ::testing::UnorderedPointwise(::testing::Eq(), expectedIpsVec);
  ASSERT_THAT(ipsVec, m);
}
