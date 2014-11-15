#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <weechat-plugin.h>

#include "api-rpc.h"
#include "utility-application.h"
#include "miner-plugin.h"

#define RECVSIZE 65500

#define RPC_COMMAND_VERSION "{\"command\":\"version\"}"

bool rpc_is_address_server(struct sockaddr_in target_address)
{
    int socket_fd;
    bool result;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        application_fail();

    result = connect(socket_fd, (struct sockaddr *)&target_address, sizeof(target_address)) == 0;
    if (result)
    {
        int position;
        ssize_t socket_ret;
        size_t buffer_size = RECVSIZE;
        char *buffer = malloc(buffer_size +1);

        socket_ret = send(socket_fd, RPC_COMMAND_VERSION, strlen(RPC_COMMAND_VERSION), 0);
        if (socket_ret < 0) {
            result = false;
        }
        else {
            position = 0;
            buffer[0] = '\0';
            while (true)
            {
                if (buffer_size < RECVSIZE + position)
                {
                    buffer_size *= 2;
                    buffer = realloc(buffer, buffer_size);
                    assert(buffer);
                }
                socket_ret = recv(socket_fd, &buffer[position], RECVSIZE, 0);
                if (socket_ret < 0) {
                    result = false;
                    break;
                }
                if (socket_ret == 0)
                    break;
                position += socket_ret;
                buffer[position] = '\0';
            }

            weechat_printf(NULL, "Reply was %s", buffer);
        }
    }
    close(socket_fd);

    return result;
}