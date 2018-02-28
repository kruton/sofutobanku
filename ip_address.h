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

#ifndef _SB_CLIENT_IP_ADDRESS_H
#define _SB_CLIENT_IP_ADDRESS_H

#include "unique_types.h"
#include <memory>
#include <string>

class IpAddress {
public:
  IpAddress(std::string);
  IpAddress(const char *);

  std::string get_bracketed() const;
  bool is_ipv6() const;
  std::string get_expanded() const;

  friend std::ostream &operator<<(std::ostream &os, const IpAddress &ip);

private:
  unique_addrinfo_ret ai_;

  bool resolve_and_set_address(const char *);
};

#endif /* _SB_CLIENT_IP_ADDRESS_H */
