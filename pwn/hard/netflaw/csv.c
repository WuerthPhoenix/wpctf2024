#include "netflow.h"
#include "csv.h"
#include <stdio.h>
#include <stdlib.h>

// function that opens a csv file for writing
FILE * open_csv_file(const char * filename) {
    FILE * file = fopen(filename, "a+");
    if (file == NULL) {
        fprintf(stderr, "[!] Error opening file %s\n", filename);
        exit(1);
    }
    return file;
}

// function that closes the csv file
void close_csv_file(FILE * file) {
    fclose(file);
}

// function that write a netflow v5 record to a csv file
void write_netflow_v5_to_csv(FILE * file, NetFlowV5 * netflow) {
    fprintf(file, "version,count,sys_uptime,unix_secs,unix_nsecs,flow_sequence,sampling_interval\n");
    fprintf(file, "%u,%u,%u,%u,%u,%u,%u\n", netflow->header.version, netflow->header.count, netflow->header.sys_uptime, netflow->header.unix_secs, netflow->header.unix_nsecs, netflow->header.flow_sequence, netflow->header.sampling_interval);
    fprintf(file, "src_ip,dst_ip,next_hop,input,output,packets,bytes,first,last,src_port,dst_port,src_as,dst_as\n");
    for (int i = 0; i < netflow->header.count; i++) {
        NetFlowV5Record * record = &(*netflow->records)[i];
        fprintf(file, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n", record->src_ip, record->dst_ip, record->next_hop, record->input, record->output, record->packets, record->bytes, record->first, record->last, record->src_port, record->dst_port, record->src_as, record->dst_as);
    }
}