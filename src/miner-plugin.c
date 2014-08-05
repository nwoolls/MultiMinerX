#include <stdlib.h>

#include "weechat-plugin.h"

WEECHAT_PLUGIN_NAME("double");
WEECHAT_PLUGIN_DESCRIPTION("Test plugin for WeeChat");
WEECHAT_PLUGIN_AUTHOR("Sébastien Helleu <flashcode@flashtux.org>");
WEECHAT_PLUGIN_VERSION("0.1");
WEECHAT_PLUGIN_LICENSE("GPL3");

struct t_weechat_plugin *weechat_plugin = NULL;


/* callback for command "/double" */

int
command_double_cb (void *data, struct t_gui_buffer *buffer, int argc,
                   char **argv, char **argv_eol)
{
    /* make C compiler happy */
    (void) data;
    (void) buffer;
    (void) argv;

    if (argc > 1)
    {
        weechat_command (NULL, argv_eol[1]);
        weechat_command (NULL, argv_eol[1]);
    }

    return WEECHAT_RC_OK;
}

int
weechat_plugin_init (struct t_weechat_plugin *plugin,
                     int argc, char *argv[])
{
    weechat_plugin = plugin;

    weechat_hook_command ("double",
                          "Display two times a message "
                          "or execute two times a command",
                          "message | command",
                          "message: message to display two times\n"
                          "command: command to execute two times",
                          NULL,
                          &command_double_cb, NULL);

    return WEECHAT_RC_OK;
}

int
weechat_plugin_end (struct t_weechat_plugin *plugin)
{
    /* make C compiler happy */
    (void) plugin;

    return WEECHAT_RC_OK;
}
