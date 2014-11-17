#include <stdlib.h>
#include <weechat-plugin.h>

#include "miner-plugin.h"
#include "miner-command.h"

WEECHAT_PLUGIN_NAME("miner");
WEECHAT_PLUGIN_DESCRIPTION("Cryptocurrency mining plugin for MultiMiner");
WEECHAT_PLUGIN_AUTHOR("Nathanial Woolls <nwoolls@gmail.com>");
WEECHAT_PLUGIN_VERSION("0.1");
WEECHAT_PLUGIN_LICENSE("GPL3");

struct t_weechat_plugin *weechat_miner_plugin = NULL;

int weechat_plugin_init(struct t_weechat_plugin *plugin, int argc, char *argv[])
{
	weechat_miner_plugin = plugin;

	miner_command_init();

	return WEECHAT_RC_OK;
}

int weechat_plugin_end(struct t_weechat_plugin *plugin)
{
	return WEECHAT_RC_OK;
}
