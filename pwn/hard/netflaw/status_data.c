#include "status_data.h"

#include <time.h>
#include <malloc.h>
#include <string.h>

StatusData * gen_status_data(char version[]) {
    StatusData * status_data = malloc(sizeof (StatusData));
    char * status_string = malloc(0x30);
    status_data->status_string = status_string;
    status_data->version = version;
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(status_data->time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm);

    return status_data;
}

void print_status_data(StatusData * status_data) {
    printf("%s", status_data->status_string);
}

void free_status_data(StatusData * status_data) {
    free(status_data->status_string);
    free(status_data);
}