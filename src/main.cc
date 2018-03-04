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

#include "argh.h"
#include "radius.h"

void print_usage(std::string executable_name) {
  std::cerr << "S*ftB*nk Hikari RADIUS authenticator" << std::endl;
  std::cerr << "Usage:\t" << executable_name
            << " --ip <local IP> --mac <local MAC> " << std::endl;
  std::cerr
      << "\t\t--auth-server <RADIUS server IP> --shared-secret <RADIUS secret> "
      << std::endl;
  std::cerr << "\t\t--password <RADIUS password>" << std::endl;
}

int main(int argc, char **argv) {
  auto cmdl = argh::parser(argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

  if (cmdl["help"]) {
    print_usage(cmdl[0]);
    return 1;
  }

  bool need_usage = false;
  if (cmdl("ip").str().empty()) {
    std::cerr << "Missing local IP address" << std::endl;
    need_usage = true;
  }
  if (cmdl("mac").str().empty()) {
    std::cerr << "Missing local MAC address" << std::endl;
    need_usage = true;
  }
  if (cmdl("auth-server").str().empty()) {
    std::cerr << "Missing RADIUS auth server IP address" << std::endl;
    need_usage = true;
  }
  if (cmdl("shared-secret").str().empty()) {
    std::cerr << "Missing RADIUS shared secret" << std::endl;
    need_usage = true;
  }
  if (cmdl("password").str().empty()) {
    std::cerr << "Missing RADIUS password" << std::endl;
    need_usage = true;
  }

  if (need_usage) {
    std::cerr << std::endl;
    print_usage(cmdl[0]);
    return 1;
  }

  return radius_transact(cmdl("auth-server").str(), cmdl("shared-secret").str(),
                         cmdl("ip").str(), cmdl("password").str(),
                         cmdl("mac").str());
}
