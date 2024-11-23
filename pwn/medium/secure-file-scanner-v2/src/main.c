#include <dirent.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "pagesize.h"

#define SCANNER_DEVICES_DIR "./scanner-devices"

#define DRIVER_BUFFER_SIZE 0x4000
#define MAX_DRIVER_NAME_LEN 0x40
#define MAX_USER_INPUT_BUFFER 0x40
#define MAX_SCANNER_DEVICE_NAME_LEN 0x200
#define MAX_SCANNER_DEVICES 0x08
#define N_BUILTIN_DRIVERS sizeof(builtin_drivers) / sizeof(builtin_drivers[0])

#define ACCESS_GRANTED 1
#define ACCESS_DENIED 0

int authenticated = 0;
unsigned char selected_driver = 0;

const unsigned char ceo_password_hash[32] = {
    0x22, 0x2c, 0xc7, 0xa1, 0xcf, 0x04, 0xeb, 0xb2, 0xef, 0xed, 0x8e,
    0x70, 0x6c, 0x55, 0x78, 0x2b, 0x6f, 0x23, 0x9b, 0x24, 0xfb, 0x83,
    0xc1, 0x28, 0x44, 0x3a, 0xbc, 0x31, 0x8d, 0x58, 0x29, 0xae};

unsigned char *user_driver = NULL;
const unsigned char builtin_drivers[][DRIVER_BUFFER_SIZE]
    __attribute__((aligned(PAGE_SIZE))) = {
        {0x44, 0x8a, 0x07, 0x44, 0x88, 0x06, 0xc3}, {0xc3}, {0xcc}};
const char builtin_driver_names[N_BUILTIN_DRIVERS][MAX_DRIVER_NAME_LEN] = {
    "Identität", "Nichts tun", "Fehlersuche"};

void hash_string(const char *const input,
                 unsigned char output[SHA256_DIGEST_LENGTH]) {
  SHA256((unsigned char *)input, strlen(input), output);
}

unsigned char authenticate() {
  if (authenticated) {
    printf("Already authenticated!\n");
    return ACCESS_GRANTED;
  }

  char password[MAX_USER_INPUT_BUFFER];
  printf("Enter the password: ");
  fflush(stdout);
  if (!fgets(password, sizeof(password), stdin)) {
    printf("Failed to read user input\n");
    return ACCESS_DENIED;
  }

  unsigned char hash[SHA256_DIGEST_LENGTH];
  hash_string(password, hash);
  if (memcmp(hash, ceo_password_hash, SHA256_DIGEST_LENGTH) == 0) {
    printf("Successfully authenticated!\n");
    authenticated = 1;
  } else {
    printf("Failed authentication!\n");
  }

  return authenticated;
}

void scan_file(const char **const devices, unsigned char device_index) {
  char path[MAX_SCANNER_DEVICE_NAME_LEN];
  snprintf(path, sizeof(path), "%s%s", SCANNER_DEVICES_DIR,
           devices[device_index]);
  if (strstr(path, "ceo") != NULL) {
    if (!authenticate()) {
      printf("Access denied! This accident will be reported!\n");
      return;
    }
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("Failed to open scanner stream\n");
    return;
  }

  char file_buffer[0x1000];
  char out_buffer[0x1000];
  memset(file_buffer, 0, sizeof(file_buffer));
  memset(out_buffer, 0, sizeof(out_buffer));
  if (!fread(file_buffer, sizeof(char), sizeof(file_buffer), file)) {
    printf("Failed to read from scanner\n");
    return;
  }
  size_t len = strlen(file_buffer);

  const unsigned char *loaded_driver = NULL;
  if (selected_driver < 0 || selected_driver > N_BUILTIN_DRIVERS) {
    printf("Invalid driver number");
    return;
  } else if (selected_driver == N_BUILTIN_DRIVERS)
    loaded_driver = user_driver;
  else
    loaded_driver = builtin_drivers[selected_driver];

  for (size_t i = 0; i < len; i++) {
    ((void (*)(char *, char *))loaded_driver)(&file_buffer[i], &out_buffer[i]);
  }
  out_buffer[strlen(file_buffer)] = '\0';

  printf("%s", out_buffer);

  fclose(file);
}

const char **const list_scanning_devices() {
  const char **devices =
      (const char **)malloc(MAX_SCANNER_DEVICES * sizeof(const char *));
  memset(devices, 0, MAX_SCANNER_DEVICES * sizeof(const char *));
  DIR *d;
  struct dirent *dir;
  d = opendir(SCANNER_DEVICES_DIR);
  unsigned char count = 0;
  if (d) {
    while ((dir = readdir(d)) != NULL && count < MAX_SCANNER_DEVICES) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        continue;
      char *device_name = (char *)malloc(MAX_SCANNER_DEVICE_NAME_LEN);
      snprintf(device_name, MAX_SCANNER_DEVICE_NAME_LEN, "/%s", dir->d_name);
      devices[count] = device_name;
      count++;
    }
    closedir(d);
  }
  return devices;
}

void print_devices(const char **const devices) {
  printf("List of scanning devices:\n");
  for (unsigned char i = 0; i < MAX_SCANNER_DEVICES && devices[i] != NULL;
       i++) {
    printf("%d: %s\n", i, devices[i]);
  }
}

unsigned char scan_a_device(const char **const devices) {
  char user_input[MAX_USER_INPUT_BUFFER];

  print_devices(devices);

  printf("\nSelect a device to scan: ");
  fflush(stdout);

  if (!fgets(user_input, MAX_USER_INPUT_BUFFER, stdin)) {
    printf("Failed to read user input\n");
    return 1;
  }
  unsigned char device_index = atoi(user_input);
  if (device_index >= MAX_SCANNER_DEVICES || device_index < 0 ||

      devices[device_index] == NULL) {
    printf("Invalid device index\n");
    return 1;
  }

  scan_file(devices, device_index);
  fflush(stdout);
  return 0;
}

unsigned char select_driver() {
  char user_input[MAX_USER_INPUT_BUFFER];
  unsigned char i;
  for (i = 0; i < N_BUILTIN_DRIVERS; i++) {
    printf("%d: %s\n", i, builtin_driver_names[i]);
  }
  if (user_driver != NULL) {
    printf("%d: User driver\n", i);
  }

  printf("Select a driver: ");
  fflush(stdout);

  if (!fgets(user_input, MAX_USER_INPUT_BUFFER, stdin)) {
    printf("Failed to read user input\n");
    return 1;
  }

  unsigned char driver_index = atoi(user_input);
  if (driver_index >= N_BUILTIN_DRIVERS + (user_driver == NULL ? 0 : 1) ||
      driver_index < 0) {
    printf("Invalid driver index\n");
    return 1;
  }

  if (driver_index == N_BUILTIN_DRIVERS) {
    if (user_driver == NULL) {
      printf("Fatal error: User driver not added\n");
      return 1;
    }
    selected_driver = N_BUILTIN_DRIVERS;
  } else {
    selected_driver = driver_index;
  }

  return 0;
}

#define NOSHIFT 0
#define SHL 1
#define SHR 2

unsigned char op_argsize(unsigned char base_size, unsigned char modifier) {
  switch (modifier) {
  case SHL:
    return base_size << 1;
  case SHR:
    return base_size >> 1;
  default: // NOSHIFT
    return base_size;
  }
}

unsigned char sanitize_driver(unsigned char *driver, unsigned short len) {
  for (unsigned short i = 0; i < len;) {
    unsigned char modifier = NOSHIFT;

    // arg modifier *2
    if ((driver[i] & 0b11111000) == 0x48) {
      modifier = SHL;
      i++;
    }

    // arg modifier /2
    if (driver[i] == 0x66 || driver[i] == 0x67) {
      modifier = SHR;
      i++;
    }

    // MOV r - imm
    if ((driver[i] & 0b11111000) == 0xb0) { // 8 bit reg
      i += op_argsize(1, modifier) + 1;
      goto endloop;
    }
    if ((driver[i] & 0b11111000) == 0xb8) { // 32 bit reg
      i += op_argsize(4, modifier) + 1;
      goto endloop;
    }

    // MOV r/m - imm
    if (driver[i] == 0xc6 || driver[i] == 0xc7) {
      i += 1 + 1 + 4;
      goto endloop;
    }

    // MOV r - r/m
    if ((driver[i] & 0b11111100) == 0x88) {
      i++;

      switch (driver[i] >> 6) {
      case 0: // no displacement
        if ((driver[i] & 0b00000111) == 0b00000101)
          i += 4;
        break;
      case 1: // displacement 8bit
        i += 1;
        break;
      case 2: // displacement 32bit
        i += 4;
        break;
      case 3: // no displacement
        break;
      }
      i++;
      goto endloop;
    }

    return 1;

  endloop:
    if (i > len)
      return 1;
  }

  driver[len] = '\xc3'; // ret call
  return 0;
}

unsigned char add_driver() {
  if (user_driver != NULL) {
    printf("Driver already added\n");
    return 1;
  }

  printf("How many bytes of driver code do you want to enter: ");
  fflush(stdout);

  char user_input[MAX_USER_INPUT_BUFFER];

  if (!fgets(user_input, MAX_USER_INPUT_BUFFER, stdin)) {
    printf("Failed to read user input\n");
    return 1;
  }

  unsigned short driver_len = atoi(user_input);
  if (driver_len <= 0 || driver_len > DRIVER_BUFFER_SIZE - 1) {
    printf("Invalid driver length\n");
    return 1;
  }

  user_driver = (unsigned char *)mmap(NULL, DRIVER_BUFFER_SIZE,
                                      PROT_READ | PROT_WRITE | PROT_EXEC,
                                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (user_driver == MAP_FAILED) {
    printf("Failed to allocate memory for user driver\n");
    return 1;
  }

  memset(user_driver, 0, DRIVER_BUFFER_SIZE);

  printf("Enter the driver code: ");
  fflush(stdout);

  if (!fgets((char *)user_driver, DRIVER_BUFFER_SIZE, stdin)) {
    printf("Failed to read user input\n");
    return 1;
  }

  if (sanitize_driver(user_driver, driver_len)) {
    munmap(user_driver, DRIVER_BUFFER_SIZE);
    user_driver = NULL;
    printf("Driver was found to be malicious and was not added\n");
    return 1;
  }

  return 0;
}

unsigned char choose_action() {
  char user_input[MAX_USER_INPUT_BUFFER];
  printf("\nChoose an action:\n");
  printf("1: Scan a device\n");
  printf("2: Select a driver\n");
  printf("3: Add a driver\n");
  printf("0: Exit\n");
  printf("Enter your choice: ");
  fflush(stdout);

  if (!fgets(user_input, MAX_USER_INPUT_BUFFER, stdin)) {
    printf("Failed to read user input\n");
    return 1;
  }

  switch (atoi(user_input)) {
  case 1:
    scan_a_device(list_scanning_devices());
    return 0;
  case 2:
    select_driver();
    return 0;
  case 3:
    return add_driver();
  default:
    printf("Invalid choice\n");
  case 0:
    return 1;
  }
}

int main() {
  printf("Secure File Scanner v2: now with improved security and new "
         "features!\n");
  const char **const devices = list_scanning_devices();

  if (mprotect((void *)builtin_drivers,
               PAGE_SIZE * (sizeof(builtin_drivers) / PAGE_SIZE + 1),
               PROT_READ | PROT_EXEC)) {
    printf("Failed to set memory protection\n");
    return EXIT_FAILURE;
  }

  while (!choose_action())
    ;

  for (unsigned char i = 0; i < MAX_SCANNER_DEVICES && devices[i] != NULL;
       i++) {
    free((void *)devices[i]);
  }
  free((void *)devices);

  if (user_driver != NULL) {
    munmap(user_driver, DRIVER_BUFFER_SIZE);
  }

  return EXIT_SUCCESS;
}
