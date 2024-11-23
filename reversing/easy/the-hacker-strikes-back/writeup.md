# Writeup: ANSI Escape Sequence Shell Challenge

In this CTF challenge, a service exposes a shell over a port using netcat.
The player must connect to this service and find the flag hidden in a file called flag.txt.
However, the flag is obfuscated using ANSI escape sequences, which override and hide the actual flag with other text.

### Step 1: Connecting to the Shell

```bash
nc localhost 8080
```

Once connected, the player will have access to a shell, with limited commands like `ls` and `cat`.

### Step 2: Exploring the Shell

After connecting, the player can use the `ls` command to check for any available files or directories.
Running `ls` will show the presence of the `flag.txt` file.

```bash
$ ls
bin
flag.txt
lib
```

Inside the `bin` directory there are only few binaries::

```bash 
ls bin
cat
ls
sh
```

The player can try reading the flag file using cat:

```bash
$ cat flag.txt
```

However, the output of this command will be obfuscated due to the presence of ANSI escape sequences. The output might look something like this:

```
These aren't the droids you're looking for.
```

### Step 4: Discovering the cat -e Option

The key to solving this challenge is to discover that the cat command has an option (-e) that can help in displaying control and escape characters. To find this out, the player can use cat --help to list available options.

```bash
$ cat --help
Usage: cat [OPTION]... [FILE]...
Concatenate FILE(s) to standard output.

With no FILE, or when FILE is -, read standard input.

  -A, --show-all           equivalent to -vET
  -b, --number-nonblank    number nonempty output lines, overrides -n
  -e                       equivalent to -vE
  -E, --show-ends          display $ at end of each line
  -n, --number             number all output lines
  -s, --squeeze-blank      suppress repeated empty output lines
  -t                       equivalent to -vT
  -T, --show-tabs          display TAB characters as ^I
  -u                       (ignored)
  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB
      --help        display this help and exit
      --version     output version information and exit

Examples:
  cat f - g  Output f's contents, then standard input, then g's contents.
  cat        Copy standard input to standard output.

GNU coreutils online help: <https://www.gnu.org/software/coreutils/>
Report any translation bugs to <https://translationproject.org/team/>
Full documentation <https://www.gnu.org/software/coreutils/cat>
or available locally via: info '(coreutils) cat invocation'
```

This option (cat -e) allows the player to see special characters like ANSI escape codes and better understand the hidden contents of the file.

### Step 5: Using cat -e to Reveal the Flag
Now, using the cat -e option, the player can attempt to read the flag.txt file again:

```bash
$ cat -e flag.txt
WPCTF{h1D3_1n_Pl41n_S1gh7}^[[26DThese aren't the droids you're looking for.$
```
