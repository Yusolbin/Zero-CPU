# Zero-CPU Syscall Table

Zero-CPU currently uses a mini-kernel style software interrupt convention based on `INT 80`.

## Calling Convention

```text
R1 = syscall number
R2 = argument 0 / return value
R3 = argument 1
R4 = argument 2
INT 80
```

The interrupt handler receives:

```text
R0 = interrupt vector
R1 = syscall number
R2 = argument 0
R3 = argument 1
R4 = argument 2
```

For the current mini-kernel and BIO-OS demos, the syscall interrupt vector is:

```text
80
```

## Current Syscalls

| Number | Name | Inputs | Returns / State | Effect |
|---:|---|---|---|---|
| 1 | debug output | `R2 = value` | none | writes `value` to `DebugOutputDevice` |
| 2 | memory write | `R2 = address`, `R3 = value` | none | writes `Memory[R2] = R3` |
| 3 | exit | `R2 = exit code` | `R7 = exit code` | stores exit code and halts CPU |
| 4 | timer read | none | `R2 = tick count` | reads `TimerDevice` tick count |
| 5 | timer enable | `R2 = interval`, `R3 = vector` | none | configures interval/vector and enables timer |
| 6 | timer disable | none | none | disables timer |
| 7 | timer configure | `R2 = interval`, `R3 = vector`, `R4 = payload` | none | configures timer interval/vector/payload |

## Examples

### Debug Output

```asm
MOV R1, 1
MOV R2, 72
INT 80
```

### Memory Write

```asm
MOV R1, 2
MOV R2, 500
MOV R3, 999
INT 80
```

Equivalent effect:

```text
Memory[500] = 999
```

### Exit

```asm
MOV R1, 3
MOV R2, 0
INT 80
```

### Timer Read

```asm
MOV R1, 4
INT 80
STORE [120], R2
```

### Timer Enable

```asm
MOV R1, 5
MOV R2, 8
MOV R3, 44
INT 80
```

### Timer Disable

```asm
MOV R1, 6
INT 80
```

### Timer Configure

```asm
MOV R1, 7
MOV R2, 8
MOV R3, 44
MOV R4, 888
INT 80
```

## BIO-OS Usage

The current BIO-OS demo uses these syscalls for:

```text
boot.zasm:
- syscall 1: boot debug output
- syscall 7: configure timer
- syscall 5: enable timer
- syscall 3: exit

user_program.zasm:
- syscall 1: user debug output
- syscall 4: read timer
- syscall 2: memory write

kernel.zasm:
- syscall handler dispatches by R1
- timer_handler handles TimerDevice interrupt vector 44
```

## Future Direction

Possible future improvements:

```text
- move syscall numbers into a shared assembler include/prelude
- add string output syscall
- add memory read syscall
- add process/user-mode simulation concepts
- expose syscall names in Zero-CPU Studio
```
