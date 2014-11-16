#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <weechat-plugin.h>
#include <wee-list.h>
#include <stdlib.h>

#include "miner-plugin.h"
#include "utility-application.h"
#include "utility-network.h"
#include "api-rpc.h"

void miner_network_scan()
{
    struct t_network_interface_list *interface_list;
    struct t_sockaddr_in_list *address_list;
    struct t_weelist_item *list_item;


    interface_list = weechat_list_new();
    if (!interface_list) application_fail();

    network_interface_scan(interface_list);

    weechat_printf(NULL, "Network interfaces:");
    int count = 0;

    for (list_item = interface_list->items; list_item;
         list_item = list_item->next_item)
    {
        struct t_network_interface_info *network_interface = (struct t_network_interface_info *)list_item->user_data;

        weechat_printf(NULL, "  %s:", weechat_list_string(list_item));
        weechat_printf(NULL, "    Host:      %s", network_interface->host);
        weechat_printf(NULL, "    Netmask:   %s", network_interface->netmask);
        weechat_printf(NULL, "    Broadcast: %s", network_interface->broadcast);
        weechat_printf(NULL, "    Range:     %s - %s",
                network_interface->range_start,
                network_interface->range_end);

        count++;
    }

    weechat_printf(NULL, "%d interface(s) detected", count);

    address_list = weechat_list_new();
    if (!address_list)  application_fail();

    network_port_scan(interface_list, 4028, 4030, address_list);

    weechat_printf(NULL, "", count);
    weechat_printf(NULL, "RPC servers:");
    count = 0;

    for (list_item = address_list->items; list_item;
         list_item = list_item->next_item)
    {
        struct sockaddr_in *open_address = (struct sockaddr_in *)list_item->user_data;

        struct t_rpc_reply_version *reply_version = rpc_reply_version_new();
        if(!reply_version) application_fail();

        bool is_client = rpc_is_address_server(*open_address, reply_version);

        if (is_client)
        {
            weechat_printf(NULL, "  %-20s: %-18s API%s[%s%s%s]",
                    weechat_list_string(list_item),
                    reply_version->description,
                    weechat_color("green"),
                    weechat_color("cyan"),
                    reply_version->api_version,
                    weechat_color("green"));
            count++;
        }

        rpc_reply_version_free(reply_version);
    }

    weechat_printf(NULL, "%d server(s) detected", count);

    network_address_list_free(address_list);
    network_interface_list_free(interface_list);
}

void miner_network_list(const bool details)
{
    if (details)
        weechat_printf(NULL, "details");
    else
        weechat_printf(NULL, "no-details");
}
