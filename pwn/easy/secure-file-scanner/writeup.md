# Writeup: Secure File Scanner

In this challenge, we are given a binary that scans files in a directory. If the
filename contains "ceo", you will need the CEO's password to access the file.

## Step 1: Analyzing the Binary

First, let's run the binary to see what it does. It will list a few files in the
`./scanner-devices` directory. If you select a file it will print it, unless it
has `ceo` in it's filename. In that case you will need to supply a password.

Note that the inserted password will be printed back if it's not correct.

Moreover, if we run some tools like `checksec` from the `pwntools` library,
we can see that the binary has PIE disabled, which means that the addresses are
static and the address of global variables will be the same every time.

```bash
$ checksec ./scanner
[*] './scanner'
    Arch:       amd64-64-little
    RELRO:      Partial RELRO
    Stack:      No canary found
    NX:         NX enabled
    PIE:        No PIE (0x400000)
```

## Step 2: Reverse Engineering

Let's open the binary in Ghidra and see what it does. Once found the `main` function,
we can see a loop that:

- prints the files in the directory
- reads the user input
- prints the file content

The function responsible for printing the content will first check if the filename
contains `ceo`. If it does, it will call an authentication function which will
compare the user input with the CEO's password.

If the password is correct, the file content will be printed and the binary will
remember that the user is authenticated by modifying a global variable (we will
refer to this variable as `authenticated`). Otherwise, if the password is incorrect,
it will be printed back to the user using `printf` like this:

```c
printf(user_input);
```

This is a format string vulnerability. We can use this to set the `authenticated`
variable to `1` and print the file content. To find the address of the `authenticated`
variable, we can use Ghidra to see the referenced address in the `authenticate` function.

NOTE: The password is hashed in the binary, so we can't just use it to authenticate.

## Step 3: Exploiting the Format String Vulnerability

Format strings allow printing data in various formats, but they can also be used
to write data to memory by using `%n`. This will write the number of characters
printed so far to the address specified by the argument. We can use this to write
to the `authenticated` variable.

Once we have the address of the `authenticated` variable, we need to calculate the
offset at which our buffer is located, with respect to the first address we start
printing with `%p`. We can use `%n$p` to print the address of the n-th argument.

We can see that our buffer starts at the offset 6, so we can use `%7$n` to write
to the address we will write on the buffer (note that the offset is 1 more than
the number we found because the first argument is the format string itself).

A simple string to exploit this vulnerability would be `A%7$nAAA\x24\x40\x40\x00\x00\x00\x00\x00`.

- The first `A` is to set the printed characters to 1;
- `%7$n` will write to the address specified next;
- `AAA` is to align the double word so that the address is written correctly;
- `\x24\x40\x40\x00\x00\x00\x00\x00` is the address of the `authenticated` variable backwards.
