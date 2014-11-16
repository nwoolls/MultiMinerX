#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>

#include "api-rpc.h"
#include "utility-application.h"
#include "utility-network.h"

#define READ_SIZE 65535

#define RPC_COMMAND_VERSION "{\"command\":\"version\"}"

bool rpc_parse_reply_version(char *reply_text, struct t_rpc_reply_version *reply_version)
{
    json_error_t error;
    json_t *root = json_loads(reply_text, 0, &error);
    if(!root)
    {
        weechat_printf(NULL, "JSON error on line %d: %s", error.line, error.text);
        return false;
    }

    if(!json_is_object(root))
    {
        json_decref(root);
        return false;
    }

    json_t *status_arr = json_object_get(root, "STATUS");
    if(!json_is_array(status_arr))
    {
        json_decref(root);
        return false;
    }

    json_t *status_elm = json_array_get(status_arr, 0);
    if(!json_is_object(status_elm))
    {
        json_decref(root);
        return false;
    }

    json_t *msg_elm = json_object_get(status_elm, "Msg");
    if(!json_is_string(msg_elm))
    {
        json_decref(root);
        return false;
    }

    json_t *description_elm = json_object_get(status_elm, "Description");
    if(!json_is_string(description_elm))
    {
        json_decref(root);
        return false;
    }

    json_t *version_arr = json_object_get(root, "VERSION");
    if(!json_is_array(version_arr))
    {
        json_decref(root);
        return false;
    }

    json_t *version_elm = json_array_get(version_arr, 0);
    if(!json_is_object(version_elm))
    {
        json_decref(root);
        return false;
    }

    json_t *miner_elm = json_object_get(version_elm, "CGMiner");
    if(!json_is_string(miner_elm))
    {
        json_decref(root);
        return false;
    }

    json_t *api_elm = json_object_get(version_elm, "API");
    if(!json_is_string(api_elm))
    {
        json_decref(root);
        return false;
    }

    reply_version->msg = strdup(json_string_value(msg_elm));
    reply_version->description = strdup(json_string_value(description_elm));
    reply_version->miner_version = strdup(json_string_value(miner_elm));
    reply_version->api_version = strdup(json_string_value(api_elm));

    json_decref(root);
    return true;
}

bool rpc_is_address_server(struct sockaddr_in target_address, struct t_rpc_reply_version *reply_version)
{
    int socket_fd;
    bool result = false;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) application_fail();

    if (connect(socket_fd, (struct sockaddr *)&target_address, sizeof(target_address)) == 0)
    {
        ssize_t socket_ret = send(socket_fd, RPC_COMMAND_VERSION, strlen(RPC_COMMAND_VERSION), 0);
        if (socket_ret < 0)
            result = false;
        else
        {
            char *buffer = malloc(READ_SIZE + 1);
            if (!buffer) application_fail();

            result = network_read_from_server(socket_fd, &buffer, READ_SIZE);

            if (result && reply_version)
                rpc_parse_reply_version(buffer, reply_version);

           free(buffer);
        }
    }
    close(socket_fd);

    return result;
}

void rpc_reply_version_free(struct t_rpc_reply_version *reply_version)
{
    free(reply_version->api_version);
    free(reply_version->description);
    free(reply_version->miner_version);
    free(reply_version->msg);
    free(reply_version);
}

struct t_rpc_reply_version * rpc_reply_version_new()
{
    struct t_rpc_reply_version *reply_version = malloc(sizeof(struct t_rpc_reply_version));
    memset(reply_version, 0, sizeof(*reply_version));
    return reply_version;
}
