#include <netdb.h>
#include <stddef.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <stdbool.h>
#include <weechat-plugin.h>
#include <wee-list.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdio.h>

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

    if (result != 0) application_fail();
}

void network_inet_ntoa(const uint64_t ip_address, const char host[NI_MAXHOST])
{
    struct in_addr address;
    address.s_addr = htonl(ip_address);
    strncpy((char *)host, inet_ntoa(address), NI_MAXHOST);
}

void network_inet_aton(const char host[NI_MAXHOST], struct in_addr *address)
{
    if (!inet_aton(host, address)) application_fail();
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

    if (interface->ifa_broadaddr != NULL)
        network_get_address_host(interface->ifa_broadaddr, network_interface->broadcast);

    network_get_ip_range(network_interface);
}

void network_interface_scan(struct t_network_interface_list *interface_list)
{
    const struct ifaddrs *interface;
    struct ifaddrs *interfaces;
    int i;

    if (getifaddrs(&interfaces) != 0) application_fail();

    for (interface = interfaces, i = 0; interface != NULL; interface = interface->ifa_next, i++)
    {
        if (interface->ifa_addr == NULL) continue;
        if ((interface->ifa_flags & IFF_UP) == 0) continue;
        if (interface->ifa_flags & IFF_POINTOPOINT) continue;
        if (interface->ifa_flags & IFF_LOOPBACK) continue;

        if (interface->ifa_addr->sa_family == AF_INET)
        {
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

void network_set_socket_nonblocking(int socket_fd)
{
    int result;
    if ((result = fcntl(socket_fd, F_GETFL, NULL)) < 0) application_fail();

    result |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, result) < 0) application_fail();
}

bool network_is_port_open(const char target_ip[NI_MAXHOST], uint16_t target_port)
{
    int socket_fd;
    struct sockaddr_in target_address;
    struct timeval tv;
    fd_set fdset;
    bool result = false;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) application_fail();

    target_address.sin_family = AF_INET;
    target_address.sin_port = htons(target_port);
    target_address.sin_addr.s_addr = inet_addr(target_ip);

    network_set_socket_nonblocking(socket_fd);

    connect(socket_fd, (struct sockaddr *)&target_address, sizeof(target_address));

    FD_ZERO(&fdset);
    FD_SET(socket_fd, &fdset);

    //100ms
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    if (select(socket_fd + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

        result = so_error == 0;
    }

    close(socket_fd);
    return result;
}

void network_port_scan(const struct t_network_interface_list *interface_list,
        uint16_t port_start, uint16_t port_end,
        struct t_sockaddr_in_list *address_list)
{
    struct t_weelist *scanned_list;
    struct t_weelist_item *interface_item;

    scanned_list = weechat_list_new();
    if (!scanned_list) application_fail();

    for (interface_item = interface_list->items; interface_item;
         interface_item = interface_item->next_item)
    {
        struct t_network_interface_info *network_interface = (struct t_network_interface_info *) interface_item->user_data;
        struct in_addr range_start_address, range_end_address;
        in_addr_t range_start_l, range_end_l;

        network_inet_aton(network_interface->range_start, &range_start_address);
        network_inet_aton(network_interface->range_end, &range_end_address);

        range_start_l = ntohl(range_start_address.s_addr);
        range_end_l = ntohl(range_end_address.s_addr);

        for (in_addr_t target_ip_l = range_start_l; target_ip_l <= range_end_l; ++target_ip_l)
        {
            for (uint16_t target_port = port_start; target_port <= port_end; ++target_port)
            {
                char target_ip[NI_MAXHOST];
                char target_address[NI_MAXHOST + 6]; //max port is 65535

                network_inet_ntoa(target_ip_l, target_ip);
                sprintf(target_address, "%s:%d", target_ip, target_port);

                //guard against scanning the same IP:port combo repeatedly
                if (weechat_list_search(scanned_list, target_address))
                    continue;
                weechat_list_add(scanned_list, target_address, WEECHAT_LIST_POS_END, NULL);

                if (network_is_port_open(target_ip, target_port))
                {
                    struct sockaddr_in *open_address = malloc(sizeof(struct sockaddr_in));
                    open_address->sin_family = AF_INET;
                    open_address->sin_addr.s_addr = htonl(target_ip_l);
                    open_address->sin_port = htons(target_port);
                    weechat_list_add(address_list, target_address, WEECHAT_LIST_POS_END, open_address);
                }
            }
        }
    }

    weechat_list_free(scanned_list);
}

void network_address_list_free(struct t_sockaddr_in_list *address_list)
{
    struct t_weelist_item *address_item;
    for (address_item = address_list->items; address_item;
         address_item = address_item->next_item)
    {
        struct sockaddr_in *network_address = (struct sockaddr_in *) address_item->user_data;
        free(network_address);
    }

    weechat_list_free(address_list);
}
