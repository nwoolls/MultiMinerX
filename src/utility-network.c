#include <netdb.h>
#include <stddef.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <weechat-plugin.h>
#include <wee-list.h>

#include "utility-application.h"
#include "utility-network.h"
#include "miner-plugin.h"

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

void network_inet_ntoa(const uint64_t ip_address, const char host[NI_MAXHOST])
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

    network_inet_ntoa(network_l + 1, network_interface->range_start);
    network_inet_ntoa(broadcast_l - 1, network_interface->range_end);
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

void network_interface_scan(struct t_network_interface_list *interface_list)
{
    const struct ifaddrs *interface;
    struct ifaddrs *interfaces;
    int i;

    if (getifaddrs(&interfaces) != 0) {
        application_fail();
    }

    for (interface = interfaces, i = 0; interface != NULL; interface = interface->ifa_next, i++) {

        if (interface->ifa_addr == NULL) continue;
        if ((interface->ifa_flags & IFF_UP) == 0) continue;
        if (interface->ifa_flags & IFF_POINTOPOINT) continue;
        if (interface->ifa_flags & IFF_LOOPBACK) continue;

        if (interface->ifa_addr->sa_family == AF_INET) {

            struct t_network_interface_info *network_interface = malloc(sizeof(struct t_network_interface_info));
            network_get_network_interface(interface, network_interface);
            weechat_list_add(interface_list, interface->ifa_name, WEECHAT_LIST_POS_END, network_interface);

        }
    }

    freeifaddrs(interfaces);
}

void network_interface_list_free(struct t_network_interface_list *interface_list)
{
    struct t_weelist_item *interface_item;
    for (interface_item = interface_list->items; interface_item;
         interface_item = interface_item->next_item)
    {
        struct t_network_interface_info *network_interface = (struct t_network_interface_info *) interface_item->user_data;
        free(network_interface);
    }

    weechat_list_free(interface_list);
}
