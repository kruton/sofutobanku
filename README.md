# sofutobanku
Setup utility for a certain Hikari provider in Japan

## Normal setup flow

This section discusses the flow needed to fully set up the Internet connection
and have all the information necessary to bring up the SIP connection if
desired.

1. ICMPv6 Router Solicitation
   1. Receive MTU information from router (i.e., 1500 bytes; see section
      2.4.2.1.5 of [FLETS])
1. IPv4 DHCP exchange (NTT SIP network)
   1. Local address for use with SIP
   1. SIP server address
   1. Static route for SIP network
1. IPv6 DHCP exchange (Internet)
   * Request:
      1. Client ID must be of the DUID-LL (Link Layer) type (see section
         2.4.2.1.4 of [FLETS])
         * Format is `00:03:00:01:<6-byte MAC address>`
      1. Request should include Prefix Delegation (PD) (see section 2.4.2.1.2
         of [FLETS])
   * Response:
      1. Vendor-specific information (NTT):
         1. MAC address (option 201)
         1. Hikari denwa telephone number (option 202)
         1. SIP domain (option 204)
         1. Route information (option 210; not needed?)
      1. Identity Assocation for Prefix Delegation (IA-PD)
         * Sends a /56 network
         * Internal LAN address should be set to PD prefix in this format:
           `xxxx:xxxx:xxxx:xx00:1111:1111:1111:1111/64` (*not* on the interface
           it received the delegation from)
1. IPv6 RADIUS exchange (IPv4-in-IPv6 setup)
   * Access-Request (1) packet:
      1. RADIUS Shared Secret and Password is needed
      1. Contains IA-PD prefix as username
         * Format is `xxxx:xxxx:xxxx:xx00:1111:1111:1111:1111`
      1. Must contain Vendor Specific Attributes (VSA)
         * MAC Address (1)
         * Client manufacturer (2)
         * Client software version (3)
         * Client hardware revision (4)
      1. CHAP authentication
         * Uses CHAP-Challenge (60) attribute
         * Password is shared among all clients
   * Access-Accept (2) packet:
      1. Contains Vendor Specific Attributes (VSA)
         * IPv4-in-IPv6 tunnel local IPv4 address (204)
         * IPv4-in-IPv6 tunnel endpoint IPv6 address (207)
      1. Other attributes don't appear to be useful
1. IPv6-in-IPv4 tunnel setup
   1. Use IPv4-in-IPv6 parameters discovered in IPv6 RADIUS exchange
   1. Must NOT have [Tunnel Encapsulation Limit Option][tunnel-encap]
      * Requires NetworkManager 1.12 or newer ([link to bug][nm-bug])

[FLETS]: https://flets.com/pdf/ip-int-flets-3.pdf "IP Network Service Interface for FLETS"
[tunnel-encap]: https://tools.ietf.org/html/rfc2473#page-13 "Tunnel Encapsulation Limit Option"
[nm-bug]: https://bugzilla.gnome.org/show_bug.cgi?id=791846 "NetworkManager bug"
