#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <ifaddrs.h>
#include <weechat-plugin.h>
#include <wee-list.h>

#include "miner-plugin.h"
#include "utility-application.h"
#include "utility-network.h"

void miner_network_scan()
{
    struct ifaddrs *interfaces;
    struct t_weelist *list;
    struct t_weelist_item *list_item;

    if (getifaddrs(&interfaces) != 0) {
        application_fail();
    }


    list = weechat_list_new();
    if (!list) {
        application_fail();
    }

    network_interface_scan(interfaces, list);

    for (list_item = list->items; list_item;
         list_item = list_item->next_item)
    {
        struct t_network_interface_info *network_interface = (struct t_network_interface_info *) list_item->user_data;

        weechat_printf(NULL, "%s: Host[%s] Netmask[%s] Broadcast[%s] Range[%s - %s]",
                weechat_list_string(list_item),
                network_interface->host,
                network_interface->netmask,
                network_interface->broadcast,
                network_interface->range_start,
                network_interface->range_end);
    }

    freeifaddrs(interfaces);
    network_interface_list_free(list);
}

void miner_network_list(const bool details)
{
    if (details)
        weechat_printf(NULL, "details");
    else
        weechat_printf(NULL, "no-details");
}
