#include <stdio.h>

#include "udp_receiver.h"
#include "netflow.h"
#include "csv.h"
#include "status_data.h"
#include "config.h"

#define PORT 8080

int main(void) {

    printf("[i] NetFlowV5@UDP/%d [%d:%d] starting ...\n",
           PORT,
           sizeof(NetFlowV5Header),
           sizeof(NetFlowV5Record)
    );

    Config * config = getDefaultConfig();
    int sockfd = bind_udp_socket(PORT);
    char buffer[1024];

    FILE * f = open_csv_file("netflow.csv");

    while (1) {
        try_read_config_file(config);

        ssize_t bytes_received = next_flow(sockfd, buffer, sizeof(buffer));
        if (bytes_received < 0) {
            break;
        }
        NetFlowV5 * netflow = get_netflow_v5_from_data(buffer, bytes_received);
        StatusData * status_data = gen_status_data(config->version);
        print_status_data(status_data);
        if (netflow->header.version == config->supported_netflow_version) {
            print_netflow_v5(netflow);
            write_netflow_v5_to_csv(f, netflow);
        } else {
            printf("[-] not supported Netflow version %d.\n", netflow->header.version);
        }
        free_netflow_v5(netflow);
        free_status_data(status_data);
    }

    close_csv_file(f);

    return 0;
}

