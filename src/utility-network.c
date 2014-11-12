#include <netdb.h>
#include <stddef.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <ifaddrs.h>

#include "weechat-plugin.h"
#include "utility-application.h"
#include "miner-plugin.h"

struct t_network_interface_info {
    char host[NI_MAXHOST];
    char netmask[NI_MAXHOST];
    char broadcast[NI_MAXHOST];
    char range_start[NI_MAXHOST];
    char range_end[NI_MAXHOST];
};

void network_get_address_host(const struct sockaddr *address, const char host[NI_MAXHOST])
{
    int result = getnameinfo(address,
            sizeof(struct sockaddr_in),
            (char *)host,
            NI_MAXHOST,
            NULL, 0, NI_NUMERICHOST);

    if (result != 0) {
        application_fail();
    }
}

void network_ntoa(const uint64_t ip_address, const char host[NI_MAXHOST])
{
    struct in_addr address;
    address.s_addr = htonl(ip_address);
    strncpy((char *)host, inet_ntoa(address), NI_MAXHOST);
}

void network_inet_aton(const char host[NI_MAXHOST], struct in_addr *address)
{
    if (!inet_aton(host, address) ) {
        application_fail();
    }
}

void network_get_ip_range(struct t_network_interface_info *network_interface)
{
    struct in_addr host_address, netmask_address, broadcast_address;
    uint64_t host_l, netmask_l, broadcast_l, network_l;

    network_inet_aton(network_interface->host, &host_address);
    network_inet_aton(network_interface->netmask, &netmask_address);
    network_inet_aton(network_interface->broadcast, &broadcast_address);

    host_l = ntohl(host_address.s_addr);
    netmask_l = ntohl(netmask_address.s_addr);
    broadcast_l = ntohl(broadcast_address.s_addr);
    network_l = host_l & netmask_l;

    network_ntoa(network_l + 1, network_interface->range_start);
    network_ntoa(broadcast_l - 1, network_interface->range_end);
}

void network_get_network_interface(const struct ifaddrs *interface, struct t_network_interface_info *network_interface)
{
    network_get_address_host(interface->ifa_addr, network_interface->host);
    network_get_address_host(interface->ifa_netmask, network_interface->netmask);

    if (interface->ifa_broadaddr != NULL) {
        network_get_address_host(interface->ifa_broadaddr, network_interface->broadcast);
    }

    network_get_ip_range(network_interface);
}

void network_interface_scan(const struct ifaddrs *interfaces)
{
    const struct ifaddrs *interface;
    int i;
    struct t_network_interface_info network_interface;

    for (interface = interfaces, i = 0; interface != NULL; interface = interface->ifa_next, i++) {

        if (interface->ifa_addr == NULL) continue;
        if ((interface->ifa_flags & IFF_UP) == 0) continue;
        if (interface->ifa_flags & IFF_POINTOPOINT) continue;
        if (interface->ifa_flags & IFF_LOOPBACK) continue;

        if (interface->ifa_addr->sa_family == AF_INET) {

            network_get_network_interface(interface, &network_interface);

            weechat_printf(NULL, "%s: Host[%s] Netmask[%s] Broadcast[%s] Range[%s - %s]",
                    interface->ifa_name,
                    network_interface.host,
                    network_interface.netmask,
                    network_interface.broadcast,
                    network_interface.range_start,
                    network_interface.range_end);
        }
    }
}
