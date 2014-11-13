#include <netdb.h>

#ifndef MultiMiner_utility_network_h
#define MultiMiner_utility_network_h

struct t_network_interface_info {
    char host[NI_MAXHOST];
    char netmask[NI_MAXHOST];
    char broadcast[NI_MAXHOST];
    char range_start[NI_MAXHOST];
    char range_end[NI_MAXHOST];
};

extern
void network_interface_scan(const struct ifaddrs *interfaces, struct t_weelist *list);

extern
void network_interface_list_free(struct t_weelist *list);

#endif
