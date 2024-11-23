#ifndef NETFLAW_CONFIG_H
#define NETFLAW_CONFIG_H

#include <bits/stdint-uintn.h>

#define MAX_CONFIG_FILE_NAME 16
#define MAX_VERSION_LENGHT 16
#define MAX_CONFIG_FILE_SIZE 500

typedef struct {
    uint16_t supported_netflow_version;
    char version[MAX_VERSION_LENGHT];
    char config_file_name[MAX_CONFIG_FILE_NAME];
    char config_file_content[MAX_CONFIG_FILE_SIZE];
} Config;

Config * getDefaultConfig();
void try_read_config_file(Config *config);

#endif //NETFLAW_CONFIG_H
