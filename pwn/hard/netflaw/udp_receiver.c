#include "udp_receiver.h"
#include "netflow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// https://www.ibm.com/docs/en/npi/1.3.0?topic=versions-netflow-v5-formats

// receive/parse: nfcapd -E -p 8080 -l /tmp/nfcap-test
// generate: podman run -it --rm networkstatic/nflow-generator -t host.containers.internal -p 8080 -c 4

ssize_t receive_udp_packet(int sockfd, struct sockaddr_in *client_addr, socklen_t *addr_len, char *buffer, size_t buffer_size) {
    ssize_t bytes_received = recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *)client_addr, addr_len);
    if (bytes_received < 0) {
        perror("recvfrom failed");
    }
    return bytes_received;
}

// get netflow with header and records from data
NetFlowV5 * get_netflow_v5_from_data(const char *data, ssize_t length) {
    // Example processing: print the received data
    NetFlowV5 * netflow = malloc(sizeof(NetFlowV5));
    parse_netflow_v5_header((const uint8_t *) data, length, &netflow->header);
    netflow->records = malloc(sizeof(NetFlowV5Record) * netflow->header.count);
    parse_netflow_v5_records((const uint8_t *) data + HEADER_SIZE, length - HEADER_SIZE, netflow->records);
    return netflow;
}

// get netflow v5 header from data
NetFlowV5Header * get_netflow_v5_header_from_data(const char *data, ssize_t length) {
    // Example processing: print the received data
    NetFlowV5Header * header = malloc(sizeof(NetFlowV5Header));
    parse_netflow_v5_header((const uint8_t *) data, length, header);
    return header;
}

NetFlowV5Record * get_netflow_v5_records_from_data(const char *data, ssize_t length) {
    // Example processing: print the received data
    printf("Processing %zd bytes of data: %.*s\n", length, (int)length, data);
    NetFlowV5Record * record = malloc(sizeof(NetFlowV5Record));
    parse_netflow_v5_record((const uint8_t *) data, length, record);
    return record;
}

int bind_udp_socket(int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to any valid IP address and a specific port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void close_udp_socket(int sockfd) {
    close(sockfd);
}

ssize_t next_flow(int sockfd, char * buffer, int len) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Receive UDP packet
    ssize_t bytes_received = receive_udp_packet(sockfd, &client_addr, &addr_len, buffer, len);
    if (bytes_received >= 0) {
        printf("[i] Received %zd bytes via UDP\n", bytes_received);
//        process_udp_data(buffer, bytes_received);
    }

    return bytes_received;
}