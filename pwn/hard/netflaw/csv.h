#ifndef NETFLAW_CSV_H
#define NETFLAW_CSV_H

#include <stdio.h>

FILE * open_csv_file(const char * filename);
void close_csv_file(FILE * file);
void write_netflow_v5_to_csv(FILE * file, NetFlowV5 * netflow);

#endif //NETFLAW_CSV_H
