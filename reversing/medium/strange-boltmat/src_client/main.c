#include "boltmat_cli.h"
#include "boltmat_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int main() {
  int cli_to_client[2];

  if (pipe(cli_to_client) == -1)
    exit(EXIT_FAILURE);

  pid_t c_pid = fork();
  if (c_pid != 0) {
    close(cli_to_client[PIPE_READ]);
    boltmat_cli(cli_to_client[PIPE_WRITE]);
    kill(c_pid, SIGUSR1);
    waitpid(c_pid, NULL, 0);
  } else {
    close(cli_to_client[PIPE_WRITE]);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    client_loop(cli_to_client[PIPE_READ]);
  }

  exit(EXIT_SUCCESS);
}
