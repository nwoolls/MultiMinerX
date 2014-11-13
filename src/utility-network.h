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

#define t_network_interface_list t_weelist
#define t_sockaddr_in_list t_weelist

/*
 * Free a t_network_interface_list with a t_network_interface_info allocated in
 * each t_weelist_item.user_data
 */
extern
void network_interface_list_free(struct t_network_interface_list *interface_list);

/*
 * Populate a t_network_interface_list with local IPv4 network interfaces
 * t_weelist_item.data is the name of the interface
 * t_weelist_item.user_data is a t_network_interface_info with host info
 */
extern
void network_interface_scan(struct t_network_interface_list *interface_list);

/*
 * Free a t_sockaddr_in_list with a sockaddr_in allocated in each
 * t_weelist_item.user_data
 */
extern
void network_address_list_free(struct t_sockaddr_in_list *address_list);

/*
 * Scan each t_network_interface_info in interface_list from port_start to
 * port_end, populating address_list with open address/port combinations
 */
extern
void network_port_scan(const struct t_network_interface_list *interface_list,
        uint16_t port_start, uint16_t port_end,
        struct t_sockaddr_in_list *address_list);

#endif
