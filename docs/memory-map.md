# Zero-CPU Memory Map

This document defines the current memory layout used by Zero-CPU.

The goal is to prevent accidental overlap between code, data, stack, and MMIO regions.

A recent bug happened because test data was written to addresses near `520`, while binary code was loaded from address `512`. The program overwrote its own instructions and the CPU later failed with:

```text
Invalid opcode byte
```

This document exists to prevent that class of bug.

---

## 1. Current Memory Layout

Current conventional memory map:

```text
0x0000..0x01FF    Low data / scratch memory
0x0200..          Binary code load area
0x0800..          Stack area, initial SP around 2048
0xF000..0xF00F    DebugOutputDevice MMIO
0xF100..0xF12F    TimerDevice MMIO
```

Decimal version:

```text
0..511            Low data / scratch memory
512..             Binary code load area
2048..            Stack area
61440..61455      DebugOutputDevice MMIO
61696..61743      TimerDevice MMIO
```

---

## 2. Important Addresses

```text
Binary code base      = 512    = 0x0200
Default stack base    = 2048   = 0x0800
Debug MMIO base       = 61440  = 0xF000
Timer MMIO base       = 61696  = 0xF100
```

---

## 3. Region Summary

| Region | Hex Range | Decimal Range | Purpose |
|---|---:|---:|---|
| Low data / scratch | `0x0000..0x01FF` | `0..511` | small examples, test memory, syscall demo output |
| Binary code | `0x0200..` | `512..` | `.zbin` code loaded by `BinaryLoader` |
| Stack | `0x0800..` | `2048..` | stack pointer starts around this area |
| Debug MMIO | `0xF000..0xF00F` | `61440..61455` | debug output device |
| Timer MMIO | `0xF100..0xF12F` | `61696..61743` | timer device registers |

---

## 4. Rule: Keep Test Data Below Code Base

For simple examples and CLI tests, prefer data addresses below the binary code base.

Good:

```asm
STORE [100], R1
STORE [300], R2
STORE [500], R3
```

Dangerous:

```asm
STORE [520], R1
STORE [536], R2
STORE [568], R3
```

Why?

Current binary code starts at:

```text
0x0200 = 512
```

So addresses like `520`, `536`, and `568` may point into the loaded instruction bytes.

Writing to those addresses can corrupt executable code.

---

## 5. The Self-Corruption Bug

The following pattern caused a real failure:

```asm
syscall_handler:
    STORE [520], R0
    STORE [528], R1
    STORE [536], R2
```

At runtime, the syscall handler was loaded around address `536`.

The instruction:

```asm
STORE [536], R2
```

overwrote part of the handler code itself.

The first syscall appeared to work, but the second syscall jumped back into corrupted code and failed with:

```text
Invalid opcode byte
```

This is an important systems lesson:

```text
If code and data share memory, writes to the wrong address can destroy executable instructions.
```

---

## 6. Current Recommended Scratch Addresses

Use these for tests and examples:

```text
100..199    basic examples
200..299    MMIO tests / output checks
300..399    syscall and interrupt test metadata
400..499    software interrupt tests
500..511    last-resort scratch before code base
```

Avoid using:

```text
512 and above
```

for ordinary data unless the test intentionally checks code/data overlap behavior.

---

## 7. MMIO Region

MMIO means memory-mapped I/O.

Zero-CPU currently uses high memory addresses for devices.

### DebugOutputDevice

```text
Base: 0xF000
Size: 0x10
Decimal: 61440..61455
```

Registers:

```text
offset 0 = write value / read last value
offset 8 = read write count
```

Example:

```asm
MOV R1, 65
STORE [61440], R1
```

This writes `65` to `DebugOutputDevice`.

### TimerDevice

```text
Base: 0xF100
Size: 0x30
Decimal: 61696..61743
```

Registers:

```text
offset 0  = tick count
offset 8  = interval
offset 16 = enabled
offset 24 = vector
offset 32 = payload
offset 40 = interrupt count
```

---

## 8. Stack Region

The stack currently starts around:

```text
2048 = 0x0800
```

Stack operations include:

```asm
PUSH R1
POP R2
CALL function
RET
INT 80
IRET
```

Current interrupt frame is minimal:

```text
INT / interrupt delivery:
    push return address

IRET:
    pop return address
```

Future interrupt frame:

```text
push return address
push FLAGS
```

---

## 9. Code Region

Binary code is loaded at:

```text
512 = 0x0200
```

Each encoded instruction is currently fixed-size:

```text
24 bytes
```

So instruction addresses are:

```text
instruction 0 = 512
instruction 1 = 536
instruction 2 = 560
instruction 3 = 584
...
```

This explains why writing to `536` was dangerous.

`536` is not just a random data address. It can be the second loaded instruction.

---

## 10. Future Improvements

Near-term improvements:

```text
- centralize memory map constants in code
- update tests to use named constants where possible
- avoid magic numbers in CLI tests
- add loader validation for unsafe data writes later
```

Possible future memory layout:

```text
0x0000..0x01FF    reserved / zero page
0x0200..0x07FF    user program code
0x0800..0x0FFF    stack
0x1000..0x7FFF    heap / user data
0x8000..0xBFFF    kernel code
0xC000..0xEFFF    kernel data / device tables
0xF000..0xFFFF    MMIO
```

This would move Zero-CPU closer to a mini operating-system style architecture.

---

## 11. Current Rule

For now, follow this rule:

```text
Use 0..511 for simple data and test metadata.
Use 512+ for loaded binary code.
Use 2048+ for stack.
Use 0xF000+ for MMIO.
```

If a program writes to the wrong region, it may corrupt itself.

That is not always a bug in the emulator.

Sometimes it is the expected result of a low-level memory model.
