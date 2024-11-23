#include "network.h"
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define IN_BUF_SIZE 0x400

struct timeval tv;

void runtime_error_occured() {
  /*
   * This function is called when a runtime error occurs.
   * It closes the server connection and sends a signal to the cli process to
   * quit.
   */

  close_server_connection();
  kill(getppid(), SIGUSR1);
  exit(EXIT_FAILURE);
}

void cli_sent_quit() {
  /*
   * This function is called when the cli process sends a quit signal USR1,
   * meaning that the user has quit the program. It closes the server connection
   * and exits the program.
   */
  close_server_connection();
  exit(EXIT_SUCCESS);
}

unsigned char read_char_from_pipe(int pipe_fd, char *const c) {
  /*
   * This function reads a single byte from the pipe_fd file descriptor and
   * stores it in the c buffer. It returns 1 if the read operation fails, and 0
   * otherwise.
   */
  if (read(pipe_fd, c, 1) != 1) {
    runtime_error_occured();
  }
  if (c[0] == 0x00)
    return 1;
  // this is not skill issue, this is ObFuScAtIoN
  c[1] = 0x00;
  return 0;
}

void send_metric_data(const char *const c) {
  /*
   * This function sends a single byte of metric data to the server. It sends
   * the data as an xml document.
   */

  // Thanks Chat GPT
  xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
  xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "metric");
  xmlDocSetRootElement(doc, root);
  xmlNewChild(root, NULL, BAD_CAST "keypress", BAD_CAST c);
  gettimeofday(&tv, NULL);
  char timestamp_buff[0x40];
  sprintf(timestamp_buff, "%ld.%ld", tv.tv_sec, tv.tv_usec);
  xmlNewChild(root, NULL, BAD_CAST "timestamp", BAD_CAST & timestamp_buff);
  xmlChar *xmlbuff;
  int buffersize;
  xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
  xmlFreeDoc(doc);

  // send xml to server
  if (send_network_data((char *)xmlbuff, buffersize))
    runtime_error_occured();
  xmlFree(xmlbuff);
}

unsigned char get_txt_node_from_node(const xmlNodePtr node,
                                     xmlNodePtr *const out_node,
                                     const char *const node_name) {
  /*
   * This function gets a child txt node from a parent node. It stores the child
   * node in the out_node buffer.
   */

  for (*out_node = node->children; *out_node; *out_node = (*out_node)->next)
    if ((*out_node)->type == XML_ELEMENT_NODE &&
        xmlStrcmp((*out_node)->name, BAD_CAST node_name) == 0)
      break;

  if (!*out_node || !(*out_node)->children ||
      (*out_node)->children->type != XML_TEXT_NODE)
    return 1;

  return 0;
}

void receive_metric_data(unsigned char *const char_count,
                         char *const checksum) {
  /*
   * This function is called when the server sends metric data to the client.
   * It prints the data to the console.
   */

  char read_buff[IN_BUF_SIZE];

  memset(read_buff, 0, IN_BUF_SIZE);
  // read response from server
  if (read_network_data(read_buff, IN_BUF_SIZE))
    runtime_error_occured();

  // parse xml response
  xmlDocPtr doc_resp =
      xmlReadMemory(read_buff, strlen(read_buff), NULL, NULL, 0);
  xmlNodePtr ack_node = xmlDocGetRootElement(doc_resp);

  // get node called status xmlNodePtr current;
  xmlNodePtr statusNode;

  if (get_txt_node_from_node(ack_node, &statusNode, "status")) {
    xmlFreeDoc(doc_resp);
    runtime_error_occured();
  }

  // check if status is success, then extract the transactionId
  char *status = (char *)statusNode->children->content;
  if (strcmp(status, "Success") == 0) {
    *char_count += 1;
    xmlNodePtr transactionIdNode;
    if (get_txt_node_from_node(ack_node, &transactionIdNode, "transactionId")) {
      xmlFreeDoc(doc_resp);
      runtime_error_occured();
    }

    char transactionId = ((char *)transactionIdNode->children->content)[0];
    *checksum ^= transactionId;
  }
  xmlFreeDoc(doc_resp);
}

void validate_transaction_checksum(unsigned char *const char_count,
                                   char *const checksum,
                                   const char expected_checksum) {
  /*
   * This function validates the checksum of the transaction. If the checksum is
   * correct, it saves it on a temporary file. Otherwise, logs the error on the
   * temporary file and exits the program.
   */

  if (*char_count == 29) {
    FILE *tmp_file = fopen("/tmp/boltmat.log", "a");
    if (tmp_file == NULL) {
      runtime_error_occured();
    }

    if (*checksum != expected_checksum) {
      fprintf(tmp_file, "Checksum failed! Calculated checksum: %d\n",
              *checksum);
      fclose(tmp_file);
      runtime_error_occured();
    } else {
      fprintf(tmp_file, "Metric checksum success!\n");
      *checksum = 0;
      *char_count = 0;
    }
    fclose(tmp_file);
  }
}

void client_loop(int pipe_fd) {
  /*
   * This function is the main client loop. It reads a single byte from the pipe
   * file descriptor and sends it to the server. As and xml document. It then
   * reads the response from the server and parses it to do stuff.
   */

  // set up signal handler for cli process quit
  signal(SIGUSR1, cli_sent_quit);

  // open server connection
  if (open_server_connection())
    runtime_error_occured();

  unsigned char char_count = 0;
  // buffer to store the read char from the cli
  char c[2];

  char checksum = 0;
  char checksum_result = 0x10;

  while (!read_char_from_pipe(pipe_fd, c)) {
    // send metric data to server
    send_metric_data(c);

    // receive metric data from server
    receive_metric_data(&char_count, &checksum);

    // check if we have received all the data correctly
    validate_transaction_checksum(&char_count, &checksum, checksum_result);
  }
  runtime_error_occured();
}
