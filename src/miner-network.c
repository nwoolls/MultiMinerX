#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/errno.h>

#include "weechat-plugin.h"
#include "miner-plugin.h"

struct miner_network_interface {
    char host[NI_MAXHOST];
    char netmask[NI_MAXHOST];
    char broadcast[NI_MAXHOST];
    char range_start[NI_MAXHOST];
    char range_end[NI_MAXHOST];
};

void miner_fail()
{
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void miner_get_address_host(const struct sockaddr *address, const char host[NI_MAXHOST])
{
    int result = getnameinfo(address,
            sizeof(struct sockaddr_in),
            (char *)host,
            NI_MAXHOST,
            NULL, 0, NI_NUMERICHOST);

    if (result != 0) {
        miner_fail();
    }
}

void miner_ntoa(const uint64_t ip_address, const char host[NI_MAXHOST])
{
    struct in_addr address;
    address.s_addr = htonl(ip_address);
    strncpy((char *)host, inet_ntoa(address), NI_MAXHOST);
}

void miner_inet_aton(const char host[NI_MAXHOST], struct in_addr *address)
{
    if (!inet_aton(host, address) ) {
        miner_fail();
    }
}

void miner_get_ip_range(struct miner_network_interface *network_interface)
{
    struct in_addr host_address, netmask_address, broadcast_address;
    uint64_t host_l, netmask_l, broadcast_l, network_l;

    miner_inet_aton(network_interface->host, &host_address);
    miner_inet_aton(network_interface->netmask, &netmask_address);
    miner_inet_aton(network_interface->broadcast, &broadcast_address);

    host_l = ntohl(host_address.s_addr);
    netmask_l = ntohl(netmask_address.s_addr);
    broadcast_l = ntohl(broadcast_address.s_addr);
    network_l = host_l & netmask_l;

    miner_ntoa(network_l + 1, network_interface->range_start);
    miner_ntoa(broadcast_l - 1, network_interface->range_end);
}

void miner_get_network_interface(const struct ifaddrs *interface, struct miner_network_interface *network_interface)
{
    miner_get_address_host(interface->ifa_addr, network_interface->host);
    miner_get_address_host(interface->ifa_netmask, network_interface->netmask);

    if (interface->ifa_broadaddr != NULL) {
        miner_get_address_host(interface->ifa_broadaddr, network_interface->broadcast);
    }

    miner_get_ip_range(network_interface);
}

void miner_interface_scan(const struct ifaddrs *interfaces)
{
    const struct ifaddrs *interface;
    int i;
    struct miner_network_interface network_interface;

    for (interface = interfaces, i = 0; interface != NULL; interface = interface->ifa_next, i++) {

        if (interface->ifa_addr == NULL) continue;
        if ((interface->ifa_flags & IFF_UP) == 0) continue;
        if (interface->ifa_flags & IFF_POINTOPOINT) continue;
        if (interface->ifa_flags & IFF_LOOPBACK) continue;

        if (interface->ifa_addr->sa_family == AF_INET) {

            miner_get_network_interface(interface, &network_interface);

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

void miner_network_scan()
{
    struct ifaddrs *interfaces;

    if (getifaddrs(&interfaces) != 0) {
        miner_fail();
    }

    miner_interface_scan(interfaces);

    freeifaddrs(interfaces);
}

void miner_network_list(const bool details)
{
    if (details)
        weechat_printf(NULL, "details");
    else
        weechat_printf(NULL, "no-details");
}