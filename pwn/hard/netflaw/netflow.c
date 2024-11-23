#include "netflow.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Function to parse a buffer of bytes into a NetFlow v5 header
int parse_netflow_v5_header(const uint8_t *buffer, size_t length, NetFlowV5Header *header) {
    if (length < HEADER_SIZE) {
        fprintf(stderr, "Buffer too small to contain a NetFlow v5 header\n");
        return -1;
    }

    memcpy(&header->version, buffer, sizeof(uint16_t));
    memcpy(&header->count, &buffer[2], sizeof(uint16_t));
    memcpy(&header->sys_uptime, &buffer[4], sizeof(uint32_t));
    memcpy(&header->unix_secs, &buffer[8], sizeof(uint32_t));
    memcpy(&header->unix_nsecs, &buffer[12], sizeof(uint32_t));
    memcpy(&header->flow_sequence, &buffer[16], sizeof(uint32_t));
    memcpy(&header->sampling_interval, &buffer[20], sizeof(uint16_t));


    // Convert fields from network byte order to host byte order
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->sys_uptime = ntohl(header->sys_uptime);
    header->unix_secs = ntohl(header->unix_secs);
    header->unix_nsecs = ntohl(header->unix_nsecs);
    header->flow_sequence = ntohl(header->flow_sequence);
    header->sampling_interval = ntohs(header->sampling_interval);

    return 0;
}

int parse_netflow_v5_records(const uint8_t *buffer, size_t length, void * records) {
    NetFlowV5Record *record = records;

    while (length > 0 ) {
        if (length < sizeof(NetFlowV5Record)) {
            fprintf(stderr, "Buffer too small to contain a NetFlow v5 record\n");
            return -1;
        }

        memcpy(record, buffer, sizeof(NetFlowV5Record));

        // Convert fields from network byte order to host byte order
        record->src_ip = ntohl(record->src_ip);
        record->dst_ip = ntohl(record->dst_ip);
        record->next_hop = ntohl(record->next_hop);
        record->input = ntohs(record->input);
        record->output = ntohs(record->output);
        record->packets = ntohl(record->packets);
        record->bytes = ntohl(record->bytes);
        record->first = ntohl(record->first);
        record->last = ntohl(record->last);
        record->src_port = ntohs(record->src_port);
        record->dst_port = ntohs(record->dst_port);
        record->src_as = ntohs(record->src_as);
        record->dst_as = ntohs(record->dst_as);

        buffer += sizeof(NetFlowV5Record);
        length -= sizeof(NetFlowV5Record);
        record++;
    }
}

// Function to parse a buffer of bytes into NetFlow v5 format
int parse_netflow_v5_record(const uint8_t *buffer, size_t length, NetFlowV5Record *record) {
    if (length < sizeof(NetFlowV5Record)) {
        fprintf(stderr, "Buffer too small to contain a NetFlow v5 record\n");
        return -1;
    }

    memcpy(record, buffer, sizeof(NetFlowV5Record));

    // Convert fields from network byte order to host byte order
    record->src_ip = ntohl(record->src_ip);
    record->dst_ip = ntohl(record->dst_ip);
    record->next_hop = ntohl(record->next_hop);
    record->input = ntohs(record->input);
    record->output = ntohs(record->output);
    record->packets = ntohl(record->packets);
    record->bytes = ntohl(record->bytes);
    record->first = ntohl(record->first);
    record->last = ntohl(record->last);
    record->src_port = ntohs(record->src_port);
    record->dst_port = ntohs(record->dst_port);
    record->src_as = ntohs(record->src_as);
    record->dst_as = ntohs(record->dst_as);

    return 0;
}

// print netflow header
void print_netflow_v5_header(const NetFlowV5Header *header) {
    printf("NetFlow v5 Header:\n");
    printf("  Version: %u\n", header->version);
    printf("  Count: %u\n", header->count);
    printf("  System Uptime: %u\n", header->sys_uptime);
    printf("  Unix Seconds: %u\n", header->unix_secs);
    printf("  Unix Nanoseconds: %u\n", header->unix_nsecs);
    printf("  Flow Sequence: %u\n", header->flow_sequence);
    printf("  Engine Type: %u\n", header->engine_type);
    printf("  Engine ID: %u\n", header->engine_id);
    printf("  Sampling Interval: %u\n", header->sampling_interval);
}

void print_netflow_v5_record(const NetFlowV5Record *record) {
    printf("NetFlow v5 Record:\n");
    printf("  Source IP: %s\n", inet_ntoa(*(struct in_addr *)&record->src_ip));
    printf("  Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&record->dst_ip));
    printf("  Next Hop: %s\n", inet_ntoa(*(struct in_addr *)&record->next_hop));
    printf("  Input Interface: %" PRIu16 "\n", record->input);
    printf("  Output Interface: %" PRIu16 "\n", record->output);
    printf("  Packets: %u\n", record->packets);
    printf("  Bytes: %u\n", record->bytes);
    printf("  First: %u\n", record->first);
    printf("  Last: %u\n", record->last);
    printf("  Source Port: %" PRIu16 "\n", record->src_port);
    printf("  Destination Port: %" PRIu16 "\n", record->dst_port);
    printf("  TCP Flags: %" PRIu8 "\n", record->tcp_flags);
    printf("  Protocol: %" PRIu8 "\n", record->protocol);
    printf("  Type of Service: %" PRIu8 "\n", record->tos);
    printf("  Source AS: %" PRIu16 "\n", record->src_as);
    printf("  Destination AS: %" PRIu16 "\n", record->dst_as);
    printf("  Source Mask: %" PRIu8 "\n", record->src_mask);
    printf("  Destination Mask: %" PRIu8 "\n", record->dst_mask);
}

// print netflow structure
void print_netflow_v5(const NetFlowV5 *netflow) {
    print_netflow_v5_header(&netflow->header);
    for (int i = 0; i < netflow->header.count; i++) {
        printf("Record %d:\n", i);
        print_netflow_v5_record(&(*netflow->records)[i]);
    }
}

// function that frees a NetFlow v5 structure
void free_netflow_v5(NetFlowV5 *netflow) {
    free(netflow->records);
    free(netflow);
}