#ifndef NETFLOP_UDP_RECEIVER_H
#define NETFLOP_UDP_RECEIVER_H

#include <arpa/inet.h>

#include "netflow.h"

ssize_t receive_udp_packet(int sockfd, struct sockaddr_in *client_addr, socklen_t *addr_len, char *buffer, size_t buffer_size);
void process_udp_data(const char *data, ssize_t length);
int bind_udp_socket(int port);
void close_udp_socket(int sockfd);
ssize_t next_flow(int sockfd, char *buffer, int len);
NetFlowV5Record * get_netflow_v5_records_from_data(const char *data, ssize_t length);
NetFlowV5 * get_netflow_v5_from_data(const char *data, ssize_t length);

#endif // NETFLOP_UDP_RECEIVER_H

