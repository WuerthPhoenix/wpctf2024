#ifndef NETFLOP_NETFLOW_H
#define NETFLOP_NETFLOW_H

#include <stdint.h>
#include <stddef.h>

// Define the NetFlow v5 record structure
typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint32_t next_hop;
    uint16_t input;
    uint16_t output;
    uint32_t packets;
    uint32_t bytes;
    uint32_t first;
    uint32_t last;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t pad1;
    uint8_t tcp_flags;
    uint8_t protocol;
    uint8_t tos;
    uint16_t src_as;
    uint16_t dst_as;
    uint8_t src_mask;
    uint8_t dst_mask;
    uint16_t pad2;
} NetFlowV5Record;

// define the netflow v5 header structure
typedef struct {
    uint16_t version;
    uint32_t sys_uptime;
    uint16_t count;
    uint32_t unix_secs;
    uint8_t engine_type;
    uint32_t unix_nsecs;
    uint8_t engine_id;
    uint32_t flow_sequence;
    uint16_t sampling_interval;
} NetFlowV5Header;

#define HEADER_SIZE 24

// define netflow v5 with header and records
typedef struct {
    NetFlowV5Header header;
    NetFlowV5Record (*records)[];
} NetFlowV5;

// Function to parse a buffer of bytes into NetFlow v5 format
int parse_netflow_v5_record(const uint8_t *buffer, size_t length, NetFlowV5Record *record);
int parse_netflow_v5_header(const uint8_t *buffer, size_t length, NetFlowV5Header *header);
void print_netflow_v5_header(const NetFlowV5Header *header);
void print_netflow_v5_record(const NetFlowV5Record *record);
int parse_netflow_v5_records(const uint8_t *buffer, size_t length, void * records);
void print_netflow_v5(const NetFlowV5 *netflow);
void free_netflow_v5(NetFlowV5 *netflow);

#endif // NETFLOP_NETFLOW_H