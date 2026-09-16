# x86-64 Assembly with NASM - Complete Course


* DEPILEMENT/DECALAGE DE LA STACK


## Table of Contents
1. [Introduction](#1-introduction)
2. [Basic Concepts](#2-basic-concepts)
3. [Registers](#3-registers)
4. [NASM Syntax & Structure](#4-nasm-syntax--structure)
5. [Data Types & Sizes](#5-data-types--sizes)
6. [Basic Instructions](#6-basic-instructions)
7. [Memory Addressing](#7-memory-addressing)
8. [Control Flow](#8-control-flow)
9. [The Stack](#9-the-stack)
10. [System Calls (Linux)](#10-system-calls-linux)
11. [Calling Convention (System V AMD64 ABI)](#11-calling-convention-system-v-amd64-abi)
12. [Writing Functions Callable from C](#12-writing-functions-callable-from-c)
13. [Practical Examples](#13-practical-examples)
14. [Debugging Tips](#14-debugging-tips)
15. [Common Pitfalls](#15-common-pitfalls)

---

## 1. Introduction

### What is Assembly?
Assembly language is a low-level programming language that has a direct correspondence to machine code. Each assembly instruction typically translates to one CPU instruction.

### Why NASM?
**NASM** (Netwide Assembler) is a popular, portable assembler that uses **Intel syntax** (as opposed to AT&T syntax used by GAS). Intel syntax is generally considered more readable.

### Intel vs AT&T Syntax

| Feature          | Intel (NASM)      | AT&T (GAS)           |
|------------------|-------------------|----------------------|
| Operand order    | `dest, src`       | `src, dest`          |
| Register prefix  | None              | `%`                  |
| Immediate prefix | None              | `$`                  |
| Memory reference | `[addr]`          | `(addr)`             |
| Size suffix      | Keyword (`dword`) | Instruction (`movl`) |

**Example - Moving 42 into eax:**
```nasm
; Intel (NASM)
mov eax, 42

; AT&T (GAS)
movl $42, %eax
```

---

## 2. Basic Concepts

### Bits and Bytes
- **Bit**: 0 or 1
- **Byte**: 8 bits (0-255 or -128 to 127)
- **Word**: 16 bits (2 bytes)
- **Double Word (dword)**: 32 bits (4 bytes)
- **Quad Word (qword)**: 64 bits (8 bytes)

### Number Representations
```nasm
mov rax, 42         ; Decimal
mov rax, 0x2A       ; Hexadecimal (0x prefix)
mov rax, 2Ah        ; Hexadecimal (h suffix)
mov rax, 101010b    ; Binary (b suffix)
mov rax, 052        ; Octal (0 prefix)
mov rax, 'A'        ; Character (ASCII value 65)
```

### Memory Layout
``` RAM
High addresses
┌─────────────────┐
│      Stack      │ ← Grows downward
├─────────────────┤
│        ↓        │
│                 │
│        ↑        │
├─────────────────┤
│      Heap       │ ← Grows upward
├─────────────────┤
│   BSS (uninit)  │
├─────────────────┤
│   Data (init)   │
├─────────────────┤
│      Text       │ ← Your code
└─────────────────┘
Low addresses
```

---

## 3. Registers

### General Purpose Registers (64-bit)
CPU

In x86-64, you have 16 general-purpose registers:

| 64-bit | 32-bit | 16-bit | 8-bit (low) | 8-bit (high) | Purpose |
|--------|--------|--------|-------------|--------------|---------|
| `rax`  | `eax`  | `ax`   | `al` 		 | 		`ah`	| Accumulator, return value |
| `rbx`  | `ebx`  | `bx`   | `bl` 		 | 		`bh`	| Base (callee-saved) |
| `rcx`  | `encx`  | `cx`   | `cl` 		 | 		`ch`	| Counter, 4th argument |
| `rdx`  | `edx`  | `dx`   | `dl` 		 | 		`dh`	| Data, 3rd argument |
| `rsi`  | `esi`  | `si`   | `sil` 		 | 		- 		| Source index, 2nd argument |
| `rdi`  | `edi`  | `di`   | `dil` 		 | 		- 		| Destination index, 1st argument |
| `rbp`  | `ebp`  | `bp`   | `bpl` 		 | 		- 		| Base pointer (callee-saved) |
| `rsp`  | `esp`  | `sp`   | `spl` 		 | 		- 		| Stack pointer |
| `r8`   | `r8d`  | `r8w`  | `r8b` 		 | 		- 		| 5th argument |
| `r9`   | `r9d`  | `r9w`  | `r9b` 		 | 		- 		| 6th argument |
| `r10`  | `r10d` | `r10w` | `r10b` 	 | 		- 		| Caller-saved |
| `r11`  | `r11d` | `r11w` | `r11b` 	 | 		- 		| Caller-saved |
| `r12`  | `r12d` | `r12w` | `r12b` 	 | 		- 		| Callee-saved |
| `r13`  | `r13d` | `r13w` | `r13b` 	 | 		- 		| Callee-saved |
| `r14`  | `r14d` | `r14w` | `r14b` 	 | 		- 		| Callee-saved |
| `r15`  | `r15d` | `r15w` | `r15b` 	 | 		- 		| Callee-saved |

### Register Visualization
```
┌───────────────────────────────────────────────────────────────┐
│                            RAX (64 bits)                      │
├───────────────────────────────┬───────────────────────────────┤
│                               │            EAX (32 bits)      │
├───────────────────────────────┼───────────────┬───────────────┤
│                               │               │   AX (16 bits)│
├───────────────────────────────┼───────────────┼───────┬───────┤
│                               │               │AH(8b) │AL(8b) │
└───────────────────────────────┴───────────────┴───────┴───────┘
 63                            32              16       8       0
```

### Special Registers
- **RIP**: Instruction pointer (current instruction address)
- **RFLAGS**: Flags register (status flags from operations)

### Important Flags (in RFLAGS)
| Flag | Name          | Set when... |
|------|---------------|-------------|
| ZF   | Zero Flag     | Result is zero |
| SF   | Sign Flag     | Result is negative (MSB = 1) |
| CF   | Carry Flag    | Unsigned overflow/underflow |
| OF   | Overflow Flag | Signed overflow |

---

## 4. NASM Syntax & Structure

### Basic File Structure
```nasm
; Comments start with semicolon

section .data           ; Initialized data
    message db "Hello", 0
    number  dq 42

section .bss            ; Uninitialized data
    buffer resb 64      ; Reserve 64 bytes

section .text           ; Code section
    global my_function  ; Export symbol

my_function:            ; Label (function name)
    ; Your code here
    ret
```

### Sections Explained

| Section   |       Purpose       | Read | Write | Execute |
|-----------|---------------------|------|-------|---------|
| `.text`   | Code (instructions) |   ✓  |   ✗   |    ✓    |
| `.data`   | Initialized data    |   ✓  |   ✓   |    ✗    |
| `.bss`    | Uninitialized data  |   ✓  |   ✓   |    ✗    |
| `.rodata` | Read-only data      |   ✓  |   ✗   |    ✗    |

### Directives

```nasm
; Data definition directives
db  ; Define Byte (1 byte)
dw  ; Define Word (2 bytes)
dd  ; Define Double word (4 bytes)
dq  ; Define Quad word (8 bytes)

; Reservation directives (for .bss)
resb  ; Reserve Bytes
resw  ; Reserve Words
resd  ; Reserve Double words
resq  ; Reserve Quad words

; Examples
my_byte     db 0xFF              ; Single byte
my_string   db "Hello", 0        ; String with null terminator
my_array    db 1, 2, 3, 4, 5     ; Array of bytes
my_int      dd 12345             ; 32-bit integer
my_long     dq 123456789         ; 64-bit integer
buffer      resb 256             ; Reserve 256 bytes
```

### Externs and Globals
```nasm
global ft_strlen    ; Export this symbol (callable from C)
extern malloc       ; Import external symbol (from libc)
extern printf
```

---

## 5. Data Types & Sizes

### Size Specifiers
When the assembler can't determine the operand size, you must specify it:

```nasm
mov byte [rax], 0       ; Move 1 byte
mov word [rax], 0       ; Move 2 bytes
mov dword [rax], 0      ; Move 4 bytes
mov qword [rax], 0      ; Move 8 bytes
```

### Common Mistakes
```nasm
; WRONG - ambiguous size
mov [rax], 0            ; Assembler doesn't know the size!

; CORRECT
mov byte [rax], 0
mov qword [rax], 0
```

---

## 6. Basic Instructions

### Data Movement

```nasm
; MOV - Move data
mov rax, rbx            ; Register to register
mov rax, 42             ; Immediate to register
mov rax, [rbx]          ; Memory to register
mov [rax], rbx          ; Register to memory
mov qword [rax], 42     ; Immediate to memory

; MOVZX - Move with zero extension
movzx rax, byte [rsi]   ; Load byte, zero-extend to 64 bits
movzx eax, cl           ; Zero-extend cl to eax

; MOVSX - Move with sign extension
movsx rax, byte [rsi]   ; Load byte, sign-extend to 64 bits
movsxd rax, eax         ; Sign-extend dword to qword

; LEA - Load Effective Address (calculates address, doesn't access memory)
lea rax, [rbx + rcx*4]  ; rax = rbx + rcx*4 (address calculation)
lea rax, [rip + msg]    ; rax = address of msg (position independent)

; XCHG - Exchange values
xchg rax, rbx           ; Swap rax and rbx
```

### Arithmetic

```nasm
; ADD - Addition
add rax, rbx            ; rax = rax + rbx
add rax, 10             ; rax = rax + 10

; SUB - Subtraction
sub rax, rbx            ; rax = rax - rbx

; INC / DEC - Increment / Decrement
inc rax                 ; rax = rax + 1
dec rax                 ; rax = rax - 1

; NEG - Negate (two's complement)
neg rax                 ; rax = -rax

; MUL - Unsigned multiplication
mul rbx                 ; rdx:rax = rax * rbx (128-bit result)

; IMUL - Signed multiplication
imul rax, rbx           ; rax = rax * rbx
imul rax, rbx, 10       ; rax = rbx * 10
imul rax, 10            ; rax = rax * 10

; DIV - Unsigned division
; Divides rdx:rax by operand
; Result: rax = quotient, rdx = remainder
xor rdx, rdx            ; Clear rdx before division!
div rbx                 ; rax = (rdx:rax) / rbx, rdx = remainder

; IDIV - Signed division
cqo                     ; Sign-extend rax into rdx:rax
idiv rbx                ; Signed division
```

### Bitwise Operations

```nasm
; AND - Bitwise AND
and rax, rbx            ; rax = rax & rbx
and rax, 0xFF           ; Mask lower byte

; OR - Bitwise OR
or rax, rbx             ; rax = rax | rbx

; XOR - Bitwise XOR
xor rax, rax            ; Fast way to zero a register
xor rax, rbx            ; rax = rax ^ rbx

; NOT - Bitwise NOT
not rax                 ; rax = ~rax

; Shifts
shl rax, 1              ; Shift left by 1 (multiply by 2)
shl rax, cl             ; Shift left by cl
shr rax, 1              ; Logical shift right (unsigned)
sar rax, 1              ; Arithmetic shift right (preserves sign)

; Rotates
rol rax, 1              ; Rotate left
ror rax, 1              ; Rotate right
```

### Comparison & Testing

```nasm
; CMP - Compare (subtracts without storing result, sets flags)
cmp rax, rbx            ; Sets flags based on rax - rbx
cmp rax, 0              ; Check if rax is zero

; TEST - Bitwise AND without storing result
test rax, rax           ; Sets ZF if rax is 0 (faster than cmp rax, 0)
test al, 1              ; Check if lowest bit is set (odd number)
```

---

## 7. Memory Addressing

### Addressing Modes

The general formula is:
```
[base + index*scale + displacement]
```

Where:
- **base**: Any general register
- **index**: Any register except RSP
- **scale**: 1, 2, 4, or 8
- **displacement**: Constant offset

```nasm
; Direct addressing
mov rax, [0x1000]       ; Load from address 0x1000

; Register indirect
mov rax, [rbx]          ; Load from address in rbx

; Register + offset
mov rax, [rbx + 8]      ; Load from rbx + 8

; Indexed
mov rax, [rbx + rcx]    ; Load from rbx + rcx

; Scaled indexed
mov rax, [rbx + rcx*8]  ; Load from rbx + rcx*8 (array of qwords)

; Full form
mov rax, [rbx + rcx*4 + 16]  ; base + index*scale + displacement
```

### Position Independent Code (PIC)
For shared libraries and modern executables:

```nasm
; BAD - Absolute addressing (not position independent)
mov rax, [my_data]

; GOOD - RIP-relative addressing (position independent)
mov rax, [rel my_data]
lea rax, [rel my_data]

; Or set default to relative
default rel

section .data
    my_data dq 42

section .text
    mov rax, [my_data]  ; Now uses RIP-relative by default
```

---

## 8. Control Flow

### Unconditional Jump
```nasm
jmp label               ; Jump to label
jmp rax                 ; Jump to address in rax
```

### Conditional Jumps

After a `cmp` or `test` instruction:

| Instruction | Meaning | Flags Checked |
|-------------|---------|---------------|
| `je` / `jz` | Jump if equal/zero | ZF=1 |
| `jne` / `jnz` | Jump if not equal/not zero | ZF=0 |
| `jg` / `jnle` | Jump if greater (signed) | ZF=0 and SF=OF |
| `jge` / `jnl` | Jump if greater or equal (signed) | SF=OF |
| `jl` / `jnge` | Jump if less (signed) | SF≠OF |
| `jle` / `jng` | Jump if less or equal (signed) | ZF=1 or SF≠OF |
| `ja` / `jnbe` | Jump if above (unsigned) | CF=0 and ZF=0 |
| `jae` / `jnb` / `jnc` | Jump if above or equal (unsigned) | CF=0 |
| `jb` / `jnae` / `jc` | Jump if below (unsigned) | CF=1 |
| `jbe` / `jna` | Jump if below or equal (unsigned) | CF=1 or ZF=1 |
| `js` | Jump if sign (negative) | SF=1 |
| `jns` | Jump if not sign (positive) | SF=0 |
| `jo` | Jump if overflow | OF=1 |
| `jno` | Jump if no overflow | OF=0 |

### Example: If-Else
```nasm
; C equivalent: if (rax > rbx) { ... } else { ... }

    cmp rax, rbx
    jle .else           ; Jump to else if rax <= rbx
    ; If block
    ; ...
    jmp .endif
.else:
    ; Else block
    ; ...
.endif:
    ; Continue
```

### Example: While Loop
```nasm
; C equivalent: while (rax > 0) { rax--; }

.while:
    cmp rax, 0
    jle .endwhile       ; Exit if rax <= 0
    dec rax
    jmp .while
.endwhile:
```

### Example: For Loop
```nasm
; C equivalent: for (rcx = 0; rcx < 10; rcx++) { ... }

    xor rcx, rcx        ; rcx = 0
.for:
    cmp rcx, 10
    jge .endfor         ; Exit if rcx >= 10
    ; Loop body
    ; ...
    inc rcx
    jmp .for
.endfor:
```

### Conditional Set (SETcc)
Sets a byte to 1 or 0 based on condition:
```nasm
cmp rax, rbx
sete al                 ; al = 1 if rax == rbx, else 0
setg al                 ; al = 1 if rax > rbx (signed)
```

### Conditional Move (CMOVcc)
Moves only if condition is true (avoids branches):
```nasm
cmp rax, rbx
cmovg rax, rcx          ; if (rax > rbx) rax = rcx
cmove rax, rdx          ; if (rax == rbx) rax = rdx
```

---

## 9. The Stack

### Stack Basics
- Grows **downward** (toward lower addresses)
- `RSP` points to the top of the stack
- Must be **16-byte aligned** before `call` instruction

### Stack Operations
```nasm
push rax                ; RSP -= 8; [RSP] = rax
pop rax                 ; rax = [RSP]; RSP += 8

; Manual stack operations
sub rsp, 32             ; Allocate 32 bytes on stack
add rsp, 32             ; Deallocate 32 bytes

; Access stack data
mov [rsp], rax          ; Store at top of stack
mov [rsp + 8], rbx      ; Store at offset
```

### Stack Frame (Traditional)
```nasm
my_function:
    push rbp            ; Save old base pointer
    mov rbp, rsp        ; Set up new base pointer
    sub rsp, 32         ; Allocate local variables

    ; Access locals via rbp
    mov [rbp - 8], rax  ; First local variable
    mov [rbp - 16], rbx ; Second local variable

    ; Access parameters (if any on stack) via rbp + 16

    mov rsp, rbp        ; Restore stack pointer
    pop rbp             ; Restore base pointer
    ret
```

### Stack Frame (Simplified)
```nasm
my_function:
    sub rsp, 40         ; Allocate space (32 + 8 for alignment)
    
    ; Access locals via rsp
    mov [rsp], rax
    mov [rsp + 8], rbx
    
    add rsp, 40         ; Deallocate
    ret
```

---

## 10. System Calls (Linux)

### How System Calls Work
1. Put syscall number in `rax`
2. Put arguments in `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`
3. Execute `syscall` instruction
4. Return value is in `rax` (negative = error)

### Common System Calls

| Syscall | Number (rax) |  rdi      |  rsi  |  rdx  |
|---------|--------------|-----------|-------|-------|
| read    | 0            | fd        | buf   | count |
| write   | 1            | fd        | buf   | count |
| open    | 2            | filename  | flags | mode  |
| close   | 3            | fd        | -     | -     |
| exit    | 60           | exit_code | -     | -     |
| mmap    | 9            | addr      | len   | prot  |
| brk     | 12           | addr      | -     | -     |

### Example: Hello World with Syscalls
```nasm
section .data
    msg db "Hello, World!", 10    ; 10 = newline
    len equ $ - msg               ; Calculate length

section .text
    global _start

_start:
    ; write(1, msg, len)
    mov rax, 1          ; syscall: write
    mov rdi, 1          ; fd: stdout
    lea rsi, [rel msg]  ; buf: message
    mov rdx, len        ; count: length
    syscall

    ; exit(0)
    mov rax, 60         ; syscall: exit
    xor rdi, rdi        ; exit code: 0
    syscall
```

---

## 11. Calling Convention (System V AMD64 ABI)

This is the standard calling convention for Linux and macOS.

### Argument Passing
Arguments are passed in registers (in order):
1. `rdi` - 1st argument
2. `rsi` - 2nd argument
3. `rdx` - 3rd argument
4. `rcx` - 4th argument
5. `r8` - 5th argument
6. `r9` - 6th argument
7. Stack - additional arguments (right to left)

### Return Value
- **rax** - Return value (up to 64 bits)
- **rdx:rax** - For 128-bit return values

### Register Preservation

| Register  | Preserved? | Who saves? |
|-----------|------------|------------|
| `rax`     | No         | Caller     |
| `rbx`     | **Yes**    | Callee     |
| `rcx`     | No         | Caller     |
| `rdx`     | No         | Caller     |
| `rsi`     | No         | Caller     |
| `rdi`     | No         | Caller     |
| `rbp`     | **Yes**    | Callee     |
| `rsp`     | **Yes**    | Callee     |
| `r8-r11`  | No         | Caller     |
| `r12-r15` | **Yes**    | Callee     |

### Stack Alignment
- Stack must be **16-byte aligned** before `call`
- After `call`, RSP is at 8 mod 16 (return address pushed)

### Red Zone (Linux/macOS only)
- 128 bytes below RSP that won't be clobbered
- Leaf functions can use without adjusting RSP

---

## 12. Writing Functions Callable from C

### Basic Template
```nasm
section .text
    global my_function

; Prototype: int my_function(int a, int b)
my_function:
    ; rdi = first argument (a)
    ; rsi = second argument (b)
    
    mov rax, rdi        ; rax = a
    add rax, rsi        ; rax = a + b
    ret                 ; Return value in rax
```

### Function with Local Variables
```nasm
section .text
    global my_function

; Prototype: long my_function(long *ptr, long count)
my_function:
    push rbx                ; Save callee-saved register
    push r12                ; We'll use these
    sub rsp, 8              ; Align stack to 16 bytes

    mov rbx, rdi            ; rbx = ptr (saved across calls)
    mov r12, rsi            ; r12 = count (saved across calls)

    ; Do some work...
    ; Can safely call other functions here
    ; rbx and r12 will be preserved

    add rsp, 8              ; Restore stack
    pop r12                 ; Restore callee-saved
    pop rbx
    ret
```

### Calling C Functions from Assembly
```nasm
section .data
    fmt db "Value: %d", 10, 0

section .text
    global my_function
    extern printf

my_function:
    push rbx                ; Align stack (8 bytes)
    
    ; Call printf("Value: %d\n", 42)
    lea rdi, [rel fmt]      ; First arg: format string
    mov rsi, 42             ; Second arg: value
    xor eax, eax            ; AL = 0 (no floating point args)
    call printf wrt ..plt   ; Call through PLT for PIC

    pop rbx
    ret
```

### Important: Using PLT for External Calls
When calling external functions in position-independent code:
```nasm
call printf wrt ..plt       ; Correct for shared libraries
```

---

## 13. Practical Examples

### Example 1: ft_strlen
```nasm
section .text
    global ft_strlen

; size_t ft_strlen(const char *s)
; rdi = pointer to string
ft_strlen:
    xor rax, rax            ; rax = 0 (counter)
    
.loop:
    cmp byte [rdi + rax], 0 ; Check for null terminator
    je .done                ; If null, we're done
    inc rax                 ; rax++
    jmp .loop               ; Continue
    
.done:
    ret                     ; Return length in rax
```

### Example 2: ft_strlen (Optimized)
```nasm
section .text
    global ft_strlen

; size_t ft_strlen(const char *s)
ft_strlen:
    mov rax, rdi            ; Save start pointer
    
.loop:
    cmp byte [rdi], 0       ; Check for null
    je .done
    inc rdi
    jmp .loop
    
.done:
    sub rdi, rax            ; length = end - start
    mov rax, rdi            ; Return in rax
    ret
```

### Example 3: ft_strcpy
```nasm
section .text
    global ft_strcpy

; char *ft_strcpy(char *dest, const char *src)
; rdi = dest, rsi = src
ft_strcpy:
    mov rax, rdi            ; Save dest for return value
    
.loop:
    mov cl, [rsi]           ; Load byte from src
    mov [rdi], cl           ; Store byte to dest
    test cl, cl             ; Check if null terminator
    jz .done                ; If null, done
    inc rsi                 ; src++
    inc rdi                 ; dest++
    jmp .loop
    
.done:
    ret                     ; Return original dest
```

### Example 4: ft_strcmp
```nasm
section .text
    global ft_strcmp

; int ft_strcmp(const char *s1, const char *s2)
ft_strcmp:
.loop:
    movzx eax, byte [rdi]   ; Load byte from s1
    movzx ecx, byte [rsi]   ; Load byte from s2
    cmp al, cl              ; Compare bytes
    jne .diff               ; If different, calculate difference
    test al, al             ; Check for end of string
    jz .equal               ; If null, strings are equal
    inc rdi
    inc rsi
    jmp .loop
    
.diff:
    sub eax, ecx            ; Return s1[i] - s2[i]
    ret
    
.equal:
    xor eax, eax            ; Return 0
    ret
```

### Example 5: ft_write (System Call Wrapper)
```nasm
section .text
    global ft_write
    extern __errno_location

; ssize_t ft_write(int fd, const void *buf, size_t count)
ft_write:
    mov rax, 1              ; syscall: write
    syscall
    
    test rax, rax           ; Check for error
    js .error               ; If negative, handle error
    ret
    
.error:
    neg rax                 ; Make error code positive
    mov rdi, rax            ; Save error code
    
    push rdi                ; Save error code on stack
    call __errno_location wrt ..plt  ; Get errno address
    pop rdi                 ; Restore error code
    
    mov [rax], edi          ; Set errno
    mov rax, -1             ; Return -1
    ret
```

### Example 6: ft_read (System Call Wrapper)
```nasm
section .text
    global ft_read
    extern __errno_location

; ssize_t ft_read(int fd, void *buf, size_t count)
ft_read:
    xor eax, eax            ; syscall: read (0)
    syscall
    
    test rax, rax
    js .error
    ret
    
.error:
    neg rax
    mov rdi, rax
    
    push rdi
    call __errno_location wrt ..plt
    pop rdi
    
    mov [rax], edi
    mov rax, -1
    ret
```

### Example 7: ft_strdup
```nasm
section .text
    global ft_strdup
    extern malloc
    extern ft_strlen
    extern ft_strcpy

; char *ft_strdup(const char *s)
ft_strdup:
    push rbx
    push r12
    
    mov r12, rdi            ; Save s
    
    ; Get length
    call ft_strlen          ; rax = strlen(s)
    
    ; Allocate memory
    lea rdi, [rax + 1]      ; Need length + 1 for null
    call malloc wrt ..plt
    
    test rax, rax           ; Check if malloc failed
    jz .done                ; If NULL, return NULL
    
    ; Copy string
    mov rbx, rax            ; Save malloc result
    mov rdi, rax            ; dest = malloc result
    mov rsi, r12            ; src = original string
    call ft_strcpy
    
    mov rax, rbx            ; Return malloced pointer
    
.done:
    pop r12
    pop rbx
    ret
```

### Example 8: ft_atoi_base (Advanced)
```nasm
section .text
    global ft_atoi_base

; int ft_atoi_base(const char *str, const char *base)
ft_atoi_base:
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    mov r12, rdi            ; r12 = str
    mov r13, rsi            ; r13 = base
    xor r14d, r14d          ; r14 = result
    mov r15d, 1             ; r15 = sign
    
    ; Calculate base length
    mov rdi, r13
    call .base_len
    mov ebx, eax            ; ebx = base_len
    cmp ebx, 2              ; Base must be >= 2
    jl .error
    
    ; Skip whitespace
.skip_space:
    movzx eax, byte [r12]
    cmp al, ' '
    je .next_space
    cmp al, 9               ; Tab
    jb .check_sign
    cmp al, 13
    ja .check_sign
.next_space:
    inc r12
    jmp .skip_space
    
.check_sign:
    cmp al, '-'
    jne .not_minus
    neg r15d                ; Flip sign
    inc r12
    jmp .parse
.not_minus:
    cmp al, '+'
    jne .parse
    inc r12
    
.parse:
    movzx eax, byte [r12]
    test al, al
    jz .done
    
    ; Find char in base
    mov rdi, r13
    mov esi, eax
    call .find_char
    cmp eax, -1
    je .done
    
    ; result = result * base + digit
    imul r14d, ebx
    add r14d, eax
    inc r12
    jmp .parse
    
.done:
    imul eax, r14d, 1
    imul eax, r15d
    jmp .return
    
.error:
    xor eax, eax
    
.return:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    ret

; Helper: get base length
.base_len:
    xor eax, eax
.bl_loop:
    cmp byte [rdi + rax], 0
    je .bl_done
    inc eax
    jmp .bl_loop
.bl_done:
    ret

; Helper: find char in base, return index or -1
.find_char:
    xor eax, eax
.fc_loop:
    movzx ecx, byte [rdi + rax]
    test cl, cl
    jz .fc_not_found
    cmp cl, sil
    je .fc_done
    inc eax
    jmp .fc_loop
.fc_not_found:
    mov eax, -1
.fc_done:
    ret
```

---

## 14. Debugging Tips

### Using GDB
```bash
# Compile with debug symbols
nasm -f elf64 -g -F dwarf my_file.s -o my_file.o
gcc -g my_file.o -o my_program

# Debug
gdb ./my_program
```

### Common GDB Commands
```gdb
# Breakpoints
break my_function
break *0x401000

# Execution
run
stepi          # Step one instruction
nexti          # Step over call
continue

# Inspection
info registers
print $rax
print/x $rax   # Hexadecimal
x/10xb $rsp    # Examine 10 bytes at RSP
x/s $rdi       # Examine string at RDI
x/10i $rip     # Examine next 10 instructions

# Display on each step
display/x $rax
display/i $rip
```

### Debugging Checklist
1. **Segfault on memory access?**
   - Check pointer validity
   - Check array bounds
   - Check alignment requirements

2. **Wrong values?**
   - Check register sizes (al vs eax vs rax)
   - Check signed vs unsigned operations
   - Check for uninitialized registers

3. **Function call issues?**
   - Stack 16-byte aligned before call?
   - Caller-saved registers preserved?
   - Correct arguments in correct registers?

---

## 15. Common Pitfalls

### 1. Not Clearing RDX Before DIV
```nasm
; WRONG
mov rax, 100
mov rbx, 3
div rbx             ; Divides RDX:RAX by RBX!

; CORRECT
mov rax, 100
xor rdx, rdx        ; Clear RDX!
mov rbx, 3
div rbx
```

### 2. Forgetting Size Specifiers
```nasm
; WRONG
mov [rax], 0        ; What size?

; CORRECT
mov byte [rax], 0
mov qword [rax], 0
```

### 3. Using 32-bit Operations (Zero Extension)
```nasm
; Operations on 32-bit registers zero the upper 32 bits!
mov rax, 0xFFFFFFFFFFFFFFFF
mov eax, 1          ; RAX is now 1, not 0xFFFFFFFF00000001!

; Use the full register if you need to preserve upper bits
mov rax, 1
```

### 4. Stack Alignment
```nasm
; WRONG - Stack may be misaligned
my_function:
    call some_other_function    ; Crash! Stack not aligned

; CORRECT
my_function:
    push rbx                    ; Align to 16 bytes
    call some_other_function    ; OK
    pop rbx
    ret
```

### 5. Position-Independent Code
```nasm
; WRONG for shared libraries
mov rax, [my_data]

; CORRECT
lea rax, [rel my_data]
mov rax, [rel my_data]
```

### 6. Signed vs Unsigned Jumps
```nasm
; For unsigned comparison
cmp rax, rbx
ja .greater         ; Jump if above (unsigned)

; For signed comparison
cmp rax, rbx
jg .greater         ; Jump if greater (signed)
```

### 7. Forgetting to Save Callee-Saved Registers
```nasm
; WRONG
my_function:
    mov rbx, rdi        ; Clobbers rbx without saving!
    call other_func
    mov rax, rbx
    ret

; CORRECT
my_function:
    push rbx            ; Save rbx
    mov rbx, rdi
    call other_func
    mov rax, rbx
    pop rbx             ; Restore rbx
    ret
```

---

## Quick Reference Card

### Instruction Summary
| Category      | Instructions                                                    |
|---------------|-----------------------------------------------------------------|
| Data Movement | `mov`, `lea`, `movzx`, `movsx`, `push`, `pop`, `xchg`           |
| Arithmetic    | `add`, `sub`, `inc`, `dec`, `mul`, `imul`, `div`, `idiv`, `neg` |
| Bitwise       | `and`, `or`, `xor`, `not`, `shl`, `shr`, `sar`, `rol`, `ror`    |
| Comparison    | `cmp`, `test`                                                   |
| Control Flow  | `jmp`, `jcc`, `call`, `ret`, `loop`                             |
| String        | `rep`, `movsb`, `stosb`, `lodsb`, `scasb`, `cmpsb`              |

### Register Usage (System V AMD64)
| Register          | Usage                        | Preserved? |
|-------------------|------------------------------|------------|
| rax               | Return value, syscall number | No         |
| rdi               | 1st argument                 | No         |
| rsi               | 2nd argument                 | No         |
| rdx               | 3rd argument                 | No         |
| rcx               | 4th argument                 | No         |
| r8                | 5th argument                 | No         |
| r9                | 6th argument                 | No         |
| r10-r11           | Scratch                      | No         |
| rbx, rbp, r12-r15 | General purpose              | **Yes**    |
| rsp               | Stack pointer                | **Yes**    |

### Common Syscalls (Linux x86-64)
| Name  | rax | rdi  | rsi   | rdx   |
|-------|-----|------|-------|-------|
| read  | 0   | fd   | buf   | count |
| write | 1   | fd   | buf   | count |
| open  | 2   | path | flags | mode  |
| close | 3   | fd   |   -   |   -   |
| mmap  | 9   | addr | len   | prot  |
| exit  | 60  | code |   -   |   -   |

---

## Compilation Commands

```bash
# Compile assembly to object file
nasm -f elf64 my_file.s -o my_file.o

# With debug symbols
nasm -f elf64 -g -F dwarf my_file.s -o my_file.o

# Link to create executable
ld my_file.o -o my_program

# Link with libc (for malloc, printf, etc.)
gcc my_file.o -o my_program

# Create shared library
gcc -shared -o libasm.so *.o
```

---

**Happy Coding!** 🚀

Remember: Assembly is about understanding how the machine works. Take your time, use GDB frequently, and don't be afraid to experiment!
