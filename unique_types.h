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

#ifndef _SB_CLIENT_UNIQUE_TYPES_H
#define _SB_CLIENT_UNIQUE_TYPES_H

#include <memory>
#include <radcli/radcli.h>

struct rc_handle_Deleter {
  void operator()(rc_handle *p) { rc_destroy(p); }
};

struct VALUE_PAIR_Deleter {
  void operator()(VALUE_PAIR *p) { rc_avpair_free(p); }
};

struct addrinfo_Deleter {
  void operator()(addrinfo *p) { freeaddrinfo(p); }
};

using unique_rc_handle = std::unique_ptr<rc_handle, rc_handle_Deleter>;
using unique_VALUE_PAIR = std::unique_ptr<VALUE_PAIR, VALUE_PAIR_Deleter>;
using unique_addrinfo_ret = std::unique_ptr<addrinfo, addrinfo_Deleter>;

#endif /* _SB_CLIENT_UNIQUE_TYPES_H */
