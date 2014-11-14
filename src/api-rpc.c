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

bool rpc_is_address_client(struct sockaddr_in target_address)
{
    int socket_fd;
    bool result;
    int socket_ret;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        application_fail();

    target_address.sin_family = AF_INET;

    socket_ret = connect(socket_fd, (struct sockaddr *)&target_address, sizeof(target_address));

    result = socket_ret == 0;

    if (result)
    {
        int p;

        size_t bufsz = RECVSIZE;
        char *buf = malloc(bufsz+1);

        socket_ret = send(socket_fd, RPC_COMMAND_VERSION, strlen(RPC_COMMAND_VERSION), 0);
        if (socket_ret < 0) {
            result = false;
        }
        else {
            p = 0;
            buf[0] = '\0';
            while (true)
            {
                if (bufsz < RECVSIZE + p)
                {
                    bufsz *= 2;
                    buf = realloc(buf, bufsz);
                    assert(buf);
                }
                socket_ret = recv(socket_fd, &buf[p], RECVSIZE, 0);
                if (socket_ret < 0) {
                    result = false;
                    break;
                }
                if (socket_ret == 0)
                    break;
                p += socket_ret;
                buf[p] = '\0';
            }


            weechat_printf(NULL, "Reply was %s", buf);
        }
    }

    close(socket_fd);
    return result;
}