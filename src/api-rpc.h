#include <netinet/in.h>
#include <stdbool.h>

#ifndef MultiMiner_api_rpc_h
#define MultiMiner_api_rpc_h

extern
bool rpc_is_address_server(struct sockaddr_in target_address);

#endif
