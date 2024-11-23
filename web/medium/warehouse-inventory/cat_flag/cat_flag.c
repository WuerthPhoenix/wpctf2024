#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Set the effective user ID to root
    if (setuid(0) != 0) {
        perror("setuid");
        exit(EXIT_FAILURE);
    }

    // Open the flag file
    FILE *file = fopen("/root/flag.txt", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Read and print the contents of the flag file
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Close the file
    fclose(file);

    return 0;
}