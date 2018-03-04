/*
 * Copyright 2018 Kenny Root
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "ip_address.h"
#include "unique_types.h"

IpAddress::IpAddress(std::string ip) { resolve_and_set_address(ip.c_str()); }

IpAddress::IpAddress(const char *ip) { resolve_and_set_address(ip); }

bool IpAddress::resolve_and_set_address(const char *ip) {
  std::unique_ptr<addrinfo> hint(new addrinfo);
  std::memset(hint.get(), 0, sizeof(addrinfo));
  hint.get()->ai_family = PF_UNSPEC;
  hint.get()->ai_flags = AI_NUMERICHOST | AI_CANONNAME;

  struct addrinfo *res;
  int ret = getaddrinfo(ip, NULL, hint.get(), &res);
  if (ret) {
    std::cerr << "ERROR: invalid address given (" << gai_strerror(ret)
              << "): " << ip << std::endl;
    return false;
  }

  ai_.reset(res);
  return true;
}

std::string IpAddress::get_bracketed() const {
  std::unique_ptr<char> hbuf(new char[NI_MAXHOST]);
  if (getnameinfo(ai_.get()->ai_addr, ai_.get()->ai_addrlen, hbuf.get(),
                  NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) != 0) {
    return "invalid";
  }

  std::string ip_converted;
  if (ai_.get()->ai_family == AF_INET6) {
    std::string bracketed;
    bracketed += '[';
    bracketed += hbuf.get();
    bracketed += ']';
    return bracketed;
  } else {
    return hbuf.get();
  }
}

std::string IpAddress::get_expanded() const {
  std::stringstream expanded;
  if (!is_ipv6()) {
    return ai_.get()->ai_canonname;
  }

  unsigned char *address_bytes =
      reinterpret_cast<sockaddr_in6 *>(ai_.get()->ai_addr)->sin6_addr.s6_addr;

  for (size_t i = 0; i < sizeof(struct in6_addr); i++) {
    if (i > 1 && (i % 2) == 0) {
      expanded << std::setw(1) << ':';
    }
    expanded << std::setw(2) << std::setfill('0') << std::hex
             << (int)address_bytes[i];
  }

  return expanded.str();
}

bool IpAddress::is_ipv6() const { return ai_.get()->ai_family == AF_INET6; }

std::ostream &operator<<(std::ostream &os, const IpAddress &ip) {
  os << ip.get_expanded();
  return os;
}
