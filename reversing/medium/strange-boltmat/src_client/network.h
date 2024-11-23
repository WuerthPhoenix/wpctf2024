#pragma once
#include <stddef.h>

unsigned char open_server_connection();

void close_server_connection();

unsigned char send_network_data(const char *const data, const size_t len);

unsigned char read_network_data(char *const buff, const size_t buff_size);
