#ifndef NETFLAW_STATUS_DATA_H
#define NETFLAW_STATUS_DATA_H

typedef struct {
    char * version;
    char * status_string;
    char time_buffer[32];
} StatusData;

StatusData * gen_status_data(char version[]);
void print_status_data(StatusData * status_data);
void free_status_data(StatusData * status_data);

#endif //NETFLAW_STATUS_DATA_H
