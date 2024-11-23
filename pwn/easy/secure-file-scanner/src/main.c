#include <dirent.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACCESS_DENIED 0
#define ACCESS_GRANTED 1

#define SCANNER_DEVICES_DIR "./scanner-devices"
#define MAX_SCANNER_DEVICES 0x08
#define MAX_SCANNER_DEVICE_NAME_LEN 0x200
#define MAX_USER_INPUT_BUFFER 0x40

const unsigned char ceo_password_hash[32] = {
    0xaa, 0x20, 0x4f, 0x8f, 0x16, 0x20, 0xbc, 0xc5, 0xca, 0x5e, 0xf3,
    0xd5, 0x5c, 0x83, 0x0c, 0x71, 0x4f, 0x66, 0x5b, 0x50, 0x29, 0x80,
    0x59, 0x76, 0x21, 0x54, 0x18, 0xcd, 0x53, 0x02, 0x67, 0x01};

int authenticated = 0;

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
    printf("Failed authentication with: ");
    printf(password);
    printf("\n");
  }

  return authenticated;
}

void scan_file(char *path) {

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

  char buffer[0x100];
  while (fgets(buffer, sizeof(buffer), file) != NULL)
    printf("%s", buffer);

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

int main() {
  char user_input[MAX_USER_INPUT_BUFFER];

  printf("Secure File Scanner\n");
  const char **const devices = list_scanning_devices();

  while (1) {
    print_devices(devices);

    printf("\nSelect a device to scan: ");
    fflush(stdout);

    if (!fgets(user_input, MAX_USER_INPUT_BUFFER, stdin)) {
      printf("Failed to read user input\n");
      return EXIT_FAILURE;
    }
    unsigned char device_index = atoi(user_input);
    if (device_index >= MAX_SCANNER_DEVICES || device_index < 0 ||
        devices[device_index] == NULL) {
      printf("Invalid device index\n");
      return EXIT_FAILURE;
    }

    char path[MAX_SCANNER_DEVICE_NAME_LEN];
    snprintf(path, sizeof(path), "%s%s", SCANNER_DEVICES_DIR,
             devices[device_index]);
    scan_file(path);
    fflush(stdout);
  }

  for (unsigned char i = 0; i < MAX_SCANNER_DEVICES && devices[i] != NULL;
       i++) {
    free((void *)devices[i]);
  }
  free((void *)devices);

  return EXIT_SUCCESS;
}
