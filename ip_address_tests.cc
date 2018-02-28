#include <sstream>

#include <gtest/gtest.h>

#include "ip_address.h"

TEST(IpAddress, GetBracketed_IPv6HasBrackets) {
  IpAddress addr("2001:db8::9:139");
  ASSERT_EQ("[2001:db8::9:139]", addr.get_bracketed());
}

TEST(IpAddress, GetBracketed_LeavesIPv4Alone) {
  IpAddress addr("127.0.0.1");
  ASSERT_EQ("127.0.0.1", addr.get_bracketed());
}

TEST(IpAddress, IsIPv6_IPv6_Success) {
  IpAddress addr("2001:db8:1000::1");
  ASSERT_TRUE(addr.is_ipv6());
}

TEST(IpAddress, IsIPv6_IPv4_Success) {
  IpAddress addr("127.0.0.1");
  ASSERT_FALSE(addr.is_ipv6());
}

TEST(IpAddress, GetExpanded_IPv6_Success) {
  IpAddress addr("2001:db8:9:100::39");
  ASSERT_EQ("2001:0db8:0009:0100:0000:0000:0000:0039", addr.get_expanded());
}

TEST(IpAddress, GetExpanded_IPv4_Same) {
  IpAddress addr("127.0.0.1");
  ASSERT_EQ("127.0.0.1", addr.get_expanded());
}

TEST(IpAddress, Stream_IPv4) {
  IpAddress addr("127.0.0.1");
  std::stringstream output;
  output << addr;
  ASSERT_EQ("127.0.0.1", output.str());
}

TEST(IpAddress, Stream_IPv6) {
  IpAddress addr("2001:db8::9:139");
  std::stringstream output;
  output << addr;
  ASSERT_EQ("2001:0db8:0000:0000:0000:0000:0009:0139", output.str());
}
