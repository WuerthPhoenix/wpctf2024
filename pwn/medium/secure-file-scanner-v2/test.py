from pwn import *

context.update(arch="amd64", os="linux")

SHOULD_PASS = [
    "mov rax, 0xffffffffffffffff",
    "mov rax, 0xffffffff",
    "mov rax, 0xffff",
    "mov rax, 0xff",
    "mov eax, 0xffffffff",
    "mov eax, 0xffff",
    "mov eax, 0xff",
    "mov ax, 0xffff",
    "mov ax, 0xff",
    "mov al, 0xff",
    "mov ah, 0xff",
    "mov rcx, 0xffffffffffffffff",
    "mov rcx, 0xffffffff",
    "mov rcx, 0xffff",
    "mov rcx, 0xff",
    "mov ecx, 0xffffffff",
    "mov ecx, 0xffff",
    "mov ecx, 0xff",
    "mov cx, 0xffff",
    "mov cx, 0xff",
    "mov cl, 0xff",
    "mov ch, 0xff",
    "mov rax, rcx",
    "mov [rax], rcx",
    "mov rax, [rcx]",
    "mov eax, ecx",
    "mov [eax], ecx",
    "mov eax, [ecx]",
    "mov [rax + 0xff], rcx",
    "mov rax, [rcx + 0xff]",
    "mov [rax + 0xffff], rcx",
    "mov [rax + 0xffff], rcx",
    "mov rax, [rcx + 0xffff]",
    "mov rax, [rcx + 0xffff]",
    "mov [rax + 0x7fffffff], rcx",
    "mov [rax + 0x7fffffff], rcx",
    "mov rax, [rcx + 0x7fffffff]",
    "mov rax, [rcx + 0x7fffffff]",
    "mov [eax + 0x7fffffff], ecx",
    "mov [eax + 0x7fffffff], ecx",
    "mov eax, [ecx + 0x7fffffff]",
    "mov eax, [ecx + 0x7fffffff]",
    "mov [rip + 0x7fffffff], rcx",
    "mov [rip + 0x7fffffff], rcx",
    "mov rax, [rip + 0x7fffffff]",
    "mov rax, [rip + 0x7fffffff]",
    "movabs rax, 0xffffffff",
]

SHOULD_FAIL = [
    "lea rax, [rsi]",
    "xor rax, rcx",
    "syscall",
    "ret",
    "int 0x80",
    "leave",
    "jmp rax",
    "call rax",
    # "inc rip",
    # "dec rip",
    "push 0xff",
    "pop rax",
]


def send_bytecode(p: process, bytecode: bytes):
    p.sendlineafter(b"choice: ", b"3")
    p.sendlineafter(b"enter: ", str(len(bytecode)).encode())
    p.sendlineafter(b"code: ", bytecode)


def test(bytecode: bytes) -> bool:
    p = process("./scanner-v2", aslr=False)
    send_bytecode(p, bytecode)
    p.recvline()
    try:
        p.recv()
    except EOFError:
        p.wait_for_close()
        return False
    else:
        p.close()
        return True


def log_test_result(result: bool, bytecode: bytes):
    if result:
        log.success("Test succeded.")
    else:
        log.failure(f"Test failed:\n{disasm(bytecode)}")


def main():
    results = []
    for i, test_entry in enumerate(SHOULD_PASS):
        log.info(f"Running test {i}")
        assembled = asm(test_entry)
        res = test(assembled)
        log_test_result(res, assembled)
        results.append(res)

    for i, test_entry in enumerate(SHOULD_FAIL):
        log.info(f"Running failing test {i}")
        assembled = asm(test_entry)
        res = not test(assembled)
        log_test_result(res, assembled)
        results.append(res)

    failed_tests = results.count(False)
    if failed_tests:
        log.failure(f"{failed_tests=}")
    else:
        log.success("ALL TEST SUCCEDED")


if __name__ == "__main__":
    main()
