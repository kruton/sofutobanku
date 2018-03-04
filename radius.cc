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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>

#include <nettle/md5.h>
#include <radcli/radcli.h>

#include "radius.h"

const std::string kManufacturer = "foxconn";
const std::string kModel = "e-wmta2.3,V5.0.0.1.rc35";
const std::string kHwRev = "hw_rev_2.00";

constexpr int kVendorSoftbank = 22197;

constexpr int kSbBBMac = 1;
constexpr int kSbBBManufacturer = 2;
constexpr int kSbBBModel = 3;
constexpr int kSbBBHWRev = 4;

constexpr int kSbIPv4LocalAddr = 204;
constexpr int kSbIPv4TunnelEndpoint = 207;

int radius_transact(const IpAddress &auth_server_ip,
                    const std::string &shared_secret, const IpAddress &username,
                    const std::string &password, const std::string &mac) {
  unique_rc_handle rh(rc_new());
  if (rh.get() == nullptr) {
    std::cerr << "ERROR: unable create new handle" << std::endl;
    return ERROR_RC;
  }

  if (rc_config_init(rh.get()) == nullptr) {
    std::cerr << "%s: unable to init rc" << std::endl;
    return ERROR_RC;
  }

  if (rc_add_config(rh.get(), "dictionary", "/etc/radcli/dictionary", "config",
                    0) != 0) {
    std::cerr << "ERROR: Unable to set dictionary" << std::endl;
    return ERROR_RC;
  }

  std::string auth_server;
  auth_server += auth_server_ip.get_bracketed();
  auth_server += "::";
  auth_server += shared_secret;
  if (rc_add_config(rh.get(), "authserver", auth_server.c_str(), "config", 0) !=
      0) {
    std::cerr << "ERROR: unable to set authserver" << std::endl;
    return ERROR_RC;
  }

  if (rc_add_config(rh.get(), "radius_retries", "3", "config", 0) != 0) {
    std::cerr << "ERROR: Unable to set radius_retries." << std::endl;
    return ERROR_RC;
  }

  if (rc_add_config(rh.get(), "radius_timeout", "5", "config", 0) != 0) {
    std::cerr << "ERROR: Unable to set radius_timeout." << std::endl;
    return ERROR_RC;
  }

  if (rc_test_config(rh.get(), "config") != 0) {
    std::cerr << "ERROR: config incomplete" << std::endl;
    return ERROR_RC;
  }

  if (rc_read_dictionary(rh.get(), rc_conf_str(rh.get(), "dictionary")) != 0) {
    std::cerr << "ERROR: Failed to initialize radius dictionary" << std::endl;
    return ERROR_RC;
  }

  if (rc_read_dictionary(rh.get(), "dictionary.softbank") != 0) {
    std::cerr << "ERROR: cannot read SoftBank dictionary" << std::endl;
    return ERROR_RC;
  }

  std::string expanded_user_ip = username.get_expanded();
  unique_VALUE_PAIR send(rc_avpair_new(rh.get(), PW_USER_NAME,
                                       expanded_user_ip.c_str(),
                                       expanded_user_ip.size(), VENDOR_NONE));
  if (send.get() == nullptr) {
    std::cerr << "%s: unable to add username" << std::endl;
    return ERROR_RC;
  }

  // Okay, this is gross, but the API sucks for this.
  VALUE_PAIR *send_raw = send.get();

  std::unique_ptr<char> challenge_bytes(new char[MD5_DIGEST_SIZE]);
  std::ifstream randin("/dev/urandom");
  randin.read(challenge_bytes.get(), MD5_DIGEST_SIZE);
  randin.close();

  std::string challenge;
  challenge.insert(0, challenge_bytes.get(), MD5_DIGEST_SIZE);
  if (rc_avpair_add(rh.get(), &send_raw, PW_CHAP_CHALLENGE, challenge.c_str(),
                    challenge.size(), VENDOR_NONE) == nullptr) {
    std::cerr << "ERROR: cannot add CHAP challenge to packet" << std::endl;
    return ERROR_RC;
  }

  std::string chap_password;
  chap_password += '\x01';
  chap_password += password;
  chap_password += challenge;

  std::unique_ptr<struct md5_ctx> ctx(new struct md5_ctx);
  md5_init(ctx.get());
  md5_update(ctx.get(), chap_password.size(),
             reinterpret_cast<const uint8_t *>(chap_password.c_str()));
  std::unique_ptr<uint8_t> hashed_bytes(new uint8_t[MD5_DIGEST_SIZE]);
  md5_digest(ctx.get(), MD5_DIGEST_SIZE, hashed_bytes.get());

  std::string response;
  response += '\x01';
  response.insert(1, reinterpret_cast<const char *>(hashed_bytes.get()),
                  MD5_DIGEST_SIZE);

  if (rc_avpair_add(rh.get(), &send_raw, PW_CHAP_PASSWORD, response.c_str(),
                    response.size(), VENDOR_NONE) == nullptr) {
    std::cerr << "ERROR: unable to add password" << std::endl;
    return ERROR_RC;
  }

  if (rc_avpair_add(rh.get(), &send_raw, kSbBBMac, mac.c_str(), mac.size(),
                    kVendorSoftbank) == nullptr) {
    std::cerr << "ERROR: unable to set MAC" << std::endl;
    return ERROR_RC;
  }

  std::string manufacturer(kManufacturer);
  if (rc_avpair_add(rh.get(), &send_raw, kSbBBManufacturer,
                    manufacturer.c_str(), manufacturer.size(),
                    kVendorSoftbank) == nullptr) {
    std::cerr << "ERROR: unable to set manufacturer" << std::endl;
    return ERROR_RC;
  }

  std::string model(kModel);
  if (rc_avpair_add(rh.get(), &send_raw, kSbBBModel, model.c_str(),
                    model.size(), kVendorSoftbank) == nullptr) {
    std::cerr << "ERROR: unable to set model" << std::endl;
    return ERROR_RC;
  }

  std::string hwrev(kHwRev);
  if (rc_avpair_add(rh.get(), &send_raw, kSbBBHWRev, hwrev.c_str(),
                    hwrev.size(), kVendorSoftbank) == nullptr) {
    std::cerr << "ERROR: unable to set HW revision" << std::endl;
    return ERROR_RC;
  }

  VALUE_PAIR *received;
  int result =
      rc_aaa(rh.get(), 0, send.get(), &received, nullptr, 0, PW_ACCESS_REQUEST);
  if (result == OK_RC) {
    unique_VALUE_PAIR vp(received);

    VALUE_PAIR *vp_local_address =
        rc_avpair_get(vp.get(), kSbIPv4LocalAddr, kVendorSoftbank);
    if (vp_local_address == nullptr) {
      std::cerr << "Local Address attribute is missing: " << kVendorSoftbank
                << "/" << kSbIPv4LocalAddr << std::endl;
    }

    VALUE_PAIR *vp_tunnel_endpoint =
        rc_avpair_get(vp.get(), kSbIPv4TunnelEndpoint, kVendorSoftbank);
    if (vp_tunnel_endpoint == nullptr) {
      std::cerr << "Tunnel Endpoint attribute is missing: " << kVendorSoftbank
                << "/" << kSbIPv4TunnelEndpoint << std::endl;
    }

    char junk[1];
    char local_address[INET_ADDRSTRLEN];
    char tunnel_endpoint[INET6_ADDRSTRLEN];

    if (rc_avpair_tostr(rh.get(), vp_local_address, junk, sizeof(junk),
                        local_address, sizeof(local_address)) ||
        rc_avpair_tostr(rh.get(), vp_tunnel_endpoint, junk, sizeof(junk),
                        tunnel_endpoint, sizeof(tunnel_endpoint))) {
      std::cerr << "Cannot find Local IPv4 Address or Tunnel Endpoint!"
                << std::endl;
      return ERROR_RC;
    }

    std::cout << local_address << " " << tunnel_endpoint << std::endl;
  } else {
    std::cerr << username << " RADIUS Authentication failure (RC=" << result
              << ")" << std::endl;
  }

  return result;
}
