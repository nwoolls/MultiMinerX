#include <stdio.h>
#include <stdbool.h>
#include <weechat-plugin.h>

#include "miner-plugin.h"
#include "miner-device.h"
#include "miner-network.h"

static
int miner_command_device(void *data, struct t_gui_buffer *buffer, int argc,
                     char **argv, char **argv_eol)
{
	if (argc < 2)
		return WEECHAT_RC_ERROR;

	if ((weechat_strcasecmp(argv[1], "list") == 0) ||
		(weechat_strcasecmp(argv[1], "listfull") == 0))
	{
		weechat_printf(NULL, "list|listfull");
	}
	else if (weechat_strcasecmp(argv[1], "probe") == 0)
	{
		weechat_printf(NULL, "probe");
	}
	else if (weechat_strcasecmp(argv[1], "scan") == 0)
	{
		weechat_printf(NULL, "scan");
		miner_device_scan();
	}
	else
		return WEECHAT_RC_ERROR;

	weechat_printf (NULL, "miner_command_device");

    return WEECHAT_RC_OK;
}

static
int miner_command_network(void *data, struct t_gui_buffer *buffer, int argc,
        char **argv, char **argv_eol)
{
    if (argc < 2)
        return WEECHAT_RC_ERROR;

    const bool list_details = weechat_strcasecmp(argv[1], "listfull") == 0;

    if ((weechat_strcasecmp(argv[1], "list") == 0) || list_details)
    {
        miner_network_list(list_details);
    }
    else if (weechat_strcasecmp(argv[1], "scan") == 0)
    {
        miner_network_scan();
    }
    else
        return WEECHAT_RC_ERROR;

    return WEECHAT_RC_OK;
}

void miner_command_init()
{
    weechat_hook_command (
						  "device",
						  N_("list, probe or scan mining devices"),
						  N_("list|listfull|probe|scan"),
						  N_("    list: list devices (without argument, this list is displayed)\n"
							 "listfull: list devices with detailed info for each device\n"
							 "   probe: probe PC hardware for mining devices (invasive)\n"
							 "    scan: scan PC hardware for mining devices (non-invasive)\n"
							 "\n"
							 "Examples:\n"),
						  NULL, &miner_command_device, NULL);


    weechat_hook_command (
            "network",
            N_("list or scan network devices"),
            N_("list|listfull|scan"),
            N_("    list: list known network devices\n"
                    "listfull: list known network devices (with details)\n"
                    "    scan: scan network for mining devices\n"
                    "\n"),
            NULL, &miner_command_network, NULL);
}
