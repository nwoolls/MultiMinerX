//
//  miner-command.c
//  MultiMiner
//
//  Created by Nathanial Woolls on 8/5/14.
//  Copyright (c) 2014 Nate Woolls. All rights reserved.
//

#include <stdio.h>
#include "weechat-plugin.h"
#include "miner-plugin.h"
#include "miner-device.h"

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
		miner_device_probe();
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

void miner_command_init()
{
    weechat_hook_command (
						  "device",
						  N_("list, probe or scan mining devices"),
						  N_("list|listfull|probe|scan"),
						  N_("    list: list known mining devices\n"
							 "listfull: list known mining devices (with details)\n"
                             "    scan: scan PC hardware for mining devices (non-invasive)\n"
							 "   probe: probe PC hardware for mining devices (invasive)\n"
							 "\n"
							 "Examples:\n"),
						  NULL, &miner_command_device, NULL);
}
