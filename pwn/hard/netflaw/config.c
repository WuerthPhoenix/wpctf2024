#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file_to_buffer(const char *file_name, char *buffer, size_t buffer_size) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, buffer_size, file);
    if (bytes_read == 0) {
        perror("Error reading file");
        return -1;
    }

    fclose(file);
    return 0;
}

void parse_config_file_content(char *content, Config *config) {
    char *line = strtok(content, "\n");
    while (line != NULL) {
        if (strncmp(line, "supported_netflow_version", 25) == 0) {
            config->supported_netflow_version = atoi(strchr(line, '=') + 1);
        } else if (strncmp(line, "config_file_name", 16) == 0) {
            strncpy(config->config_file_name, strchr(line, '=') + 1, MAX_CONFIG_FILE_NAME);
        }

        line = strtok(NULL, "\n");
    }
}

Config * getDefaultConfig() {
    Config *config = (Config *)malloc(sizeof(Config));
    if (config == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    config->supported_netflow_version = 5;
    strncpy(config->version, "4.7.1337", MAX_VERSION_LENGHT);
    strncpy(config->config_file_name, "config.ini", MAX_CONFIG_FILE_NAME);
    strncpy(config->config_file_content, "", MAX_CONFIG_FILE_SIZE);

    return config;
}

void freeConfig(Config *config) {
    free(config);
}

void try_read_config_file(Config *config) {
    if (read_file_to_buffer(config->config_file_name, config->config_file_content, MAX_CONFIG_FILE_SIZE) == 0) {
        parse_config_file_content(config->config_file_content, config);
    }
}
