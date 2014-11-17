#include <netinet/in.h>
#include <stdbool.h>

#ifndef MultiMiner_api_rpc_h
#define MultiMiner_api_rpc_h

struct t_rpc_reply_version
{
	char *msg;
	char *description;
	char *miner_version;
	char *api_version;
};

extern
bool rpc_is_address_server(struct sockaddr_in target_address, struct t_rpc_reply_version *reply_version);

extern
struct t_rpc_reply_version *rpc_reply_version_new();

extern
void rpc_reply_version_free(struct t_rpc_reply_version *reply_version);

#endif
