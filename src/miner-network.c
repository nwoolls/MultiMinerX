#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <ifaddrs.h>

#include "weechat-plugin.h"
#include "miner-plugin.h"
#include "utility-application.h"
#include "utility-network.h"

void miner_network_scan()
{
    struct ifaddrs *interfaces;

    if (getifaddrs(&interfaces) != 0) {
        application_fail();
    }

    network_interface_scan(interfaces);

    freeifaddrs(interfaces);
}

void miner_network_list(const bool details)
{
    if (details)
        weechat_printf(NULL, "details");
    else
        weechat_printf(NULL, "no-details");
}
