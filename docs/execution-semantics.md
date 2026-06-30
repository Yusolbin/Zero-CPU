# Zero-CPU Execution Semantics

## 1. Overview

이 문서는 Zero-CPU의 명령어 실행 의미론을 정의한다.

실행 의미론이란 각 명령어가 실행될 때 CPU 상태가 어떤 규칙에 따라 변화하는지를 설명하는 것이다.
Zero-CPU는 단순히 명령어 실행 결과만 확인하는 것이 아니라, 명령어 실행 전후의 CPU 상태 변화를 추적하는 것을 핵심 목표로 한다.

따라서 모든 명령어는 다음 관점에서 정의한다.

* 실행 전 CPU 상태
* 실행 명령어
* 레지스터 변화
* 메모리 변화
* 플래그 변화
* PC 변화
* SP 변화
* 예외 발생 여부

## 2. CPU State Definition

Zero-CPU의 CPU 상태는 다음과 같이 표현한다.

```text
CPUState = {
    registers,
    memory,
    flags,
    pc,
    sp,
    halted
}
```

각 구성 요소의 의미는 다음과 같다.

| 구성 요소       | 설명             |
| ----------- | -------------- |
| `registers` | 범용 레지스터 집합     |
| `memory`    | 선형 메모리 공간      |
| `flags`     | 연산 결과 상태 플래그   |
| `pc`        | 다음에 실행할 명령어 위치 |
| `sp`        | 현재 스택 위치       |
| `halted`    | CPU 실행 중지 여부   |

## 3. State Transition Model

Zero-CPU에서 명령어 실행은 하나의 상태 전이로 본다.

```text
State_before -- instruction --> State_after
```

예를 들어 다음 명령어가 있다고 하자.

```asm
ADD R1, R2
```

실행 전 상태가 다음과 같다면:

```text
R1 = 10
R2 = 20
PC = 2
```

실행 후 상태는 다음과 같다.

```text
R1 = 30
R2 = 20
PC = 3
```

즉, `ADD R1, R2`는 `R1`의 값을 변경하고, 일반 명령어이므로 `PC`를 1 증가시킨다.

## 4. General Execution Cycle

Zero-CPU는 다음 실행 사이클을 따른다.

```text
1. Fetch
2. Decode
3. Snapshot Before State
4. Execute
5. Update Flags
6. Update PC
7. Snapshot After State
8. Emit Trace Event
```

### 4.1 Fetch

`PC`가 가리키는 명령어를 가져온다.

```text
instruction = program[PC]
```

### 4.2 Decode

명령어의 opcode와 operand를 해석한다.

```text
opcode = instruction.opcode
dst = instruction.dst
src = instruction.src
```

### 4.3 Snapshot Before State

명령어 실행 전 CPU 상태를 복사한다.

```text
before = copy(cpu_state)
```

이 상태는 트레이스 로그에 기록된다.

### 4.4 Execute

명령어의 의미에 따라 CPU 상태를 변경한다.

### 4.5 Update Flags

산술, 논리, 비교 명령어의 결과에 따라 플래그를 변경한다.

### 4.6 Update PC

일반 명령어는 `PC = PC + 1`을 수행한다.
분기 명령어는 조건에 따라 `PC`를 다른 주소로 변경한다.
`HALT` 명령어는 CPU를 중지 상태로 만든다.

### 4.7 Snapshot After State

명령어 실행 후 CPU 상태를 복사한다.

```text
after = copy(cpu_state)
```

### 4.8 Emit Trace Event

실행 전 상태, 실행 명령어, 실행 후 상태를 하나의 트레이스 이벤트로 기록한다.

```text
TraceEvent = {
    before,
    instruction,
    after,
    changed_registers,
    changed_memory,
    changed_flags
}
```

## 5. Program Counter Rules

Zero-CPU에서 `PC`는 기본적으로 다음 명령어를 가리킨다.

### 5.1 Normal Instruction

일반 명령어는 실행 후 `PC`를 1 증가시킨다.

```text
PC = PC + 1
```

적용 명령어:

```text
MOV, LOAD, STORE,
ADD, SUB, MUL, DIV,
AND, OR, XOR, NOT,
CMP, TEST,
PUSH, POP,
NOP
```

### 5.2 Branch Instruction

분기 명령어는 `PC`를 특정 라벨 주소로 변경할 수 있다.

```text
PC = label_address
```

적용 명령어:

```text
JMP, JE, JNE, JG, JL
```

조건이 만족되지 않는 조건 분기 명령어는 다음 명령어로 이동한다.

```text
PC = PC + 1
```

### 5.3 Function Call

`CALL`은 복귀 주소를 스택에 저장한 뒤 함수 라벨로 이동한다.

```text
Memory[SP] = PC + 1
SP = SP + 1
PC = label_address
```

### 5.4 Return

`RET`은 스택에서 복귀 주소를 꺼내 `PC`에 저장한다.

```text
SP = SP - 1
PC = Memory[SP]
```

### 5.5 HALT

`HALT`는 CPU를 중지 상태로 만든다.

```text
halted = true
```

`HALT` 이후에는 더 이상 명령어를 실행하지 않는다.

## 6. Flag Update Rules

Zero-CPU는 초기 버전에서 `ZF`, `SF`를 우선 사용한다.

| 플래그  | 조건         |
| ---- | ---------- |
| `ZF` | 결과가 0이면 1  |
| `SF` | 결과가 음수이면 1 |

### 6.1 Zero Flag

```text
if result == 0:
    ZF = 1
else:
    ZF = 0
```

### 6.2 Sign Flag

```text
if result < 0:
    SF = 1
else:
    SF = 0
```

### 6.3 Overflow Flag

`OF`는 산술 오버플로우가 발생했을 때 설정된다.
초기 구현에서는 선택적으로 구현한다.

```text
if overflow_occurred:
    OF = 1
else:
    OF = 0
```

### 6.4 Carry Flag

`CF`는 부호 없는 연산에서 자리올림 또는 빌림이 발생했을 때 설정된다.
초기 구현에서는 선택적으로 구현한다.

```text
if carry_or_borrow_occurred:
    CF = 1
else:
    CF = 0
```

## 7. Operand Evaluation Rules

명령어의 피연산자는 실행 전에 값으로 평가된다.

### 7.1 Register Operand

```asm
R1
```

평가 결과:

```text
value = registers[R1]
```

### 7.2 Immediate Operand

```asm
10
```

평가 결과:

```text
value = 10
```

### 7.3 Memory Operand

```asm
[100]
```

평가 결과:

```text
value = memory[100]
```

### 7.4 Label Operand

```asm
loop
```

평가 결과:

```text
value = label_address_table["loop"]
```

## 8. Data Movement Semantics

## 8.1 MOV

형식:

```asm
MOV dst, src
```

의미:

```text
dst = value(src)
PC = PC + 1
```

플래그 변화:

```text
None
```

예시:

```asm
MOV R1, 10
```

상태 전이:

```text
Before:
R1 = 0
PC = 0

After:
R1 = 10
PC = 1
```

## 8.2 LOAD

형식:

```asm
LOAD dst, [address]
```

의미:

```text
dst = Memory[address]
PC = PC + 1
```

플래그 변화:

```text
None
```

예시:

```asm
LOAD R1, [100]
```

상태 전이:

```text
Before:
Memory[100] = 55
R1 = 0
PC = 0

After:
Memory[100] = 55
R1 = 55
PC = 1
```

## 8.3 STORE

형식:

```asm
STORE [address], src
```

의미:

```text
Memory[address] = value(src)
PC = PC + 1
```

플래그 변화:

```text
None
```

예시:

```asm
STORE [100], R1
```

상태 전이:

```text
Before:
R1 = 77
Memory[100] = 0
PC = 0

After:
R1 = 77
Memory[100] = 77
PC = 1
```

## 9. Arithmetic Semantics

## 9.1 ADD

형식:

```asm
ADD dst, src
```

의미:

```text
result = value(dst) + value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

예시:

```asm
ADD R1, R2
```

상태 전이:

```text
Before:
R1 = 10
R2 = 20
ZF = 0
SF = 0
PC = 2

After:
R1 = 30
R2 = 20
ZF = 0
SF = 0
PC = 3
```

## 9.2 SUB

형식:

```asm
SUB dst, src
```

의미:

```text
result = value(dst) - value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

예시:

```asm
SUB R1, R2
```

상태 전이:

```text
Before:
R1 = 10
R2 = 10
ZF = 0
SF = 0
PC = 2

After:
R1 = 0
R2 = 10
ZF = 1
SF = 0
PC = 3
```

## 9.3 MUL

형식:

```asm
MUL dst, src
```

의미:

```text
result = value(dst) * value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

예시:

```asm
MUL R1, R2
```

상태 전이:

```text
Before:
R1 = 3
R2 = 4
PC = 2

After:
R1 = 12
R2 = 4
ZF = 0
SF = 0
PC = 3
```

## 9.4 DIV

형식:

```asm
DIV dst, src
```

의미:

```text
if value(src) == 0:
    raise DivisionByZero
else:
    result = value(dst) / value(src)
    dst = result
    Update ZF
    Update SF
    PC = PC + 1
```

예시:

```asm
DIV R1, R2
```

상태 전이:

```text
Before:
R1 = 20
R2 = 4
PC = 2

After:
R1 = 5
R2 = 4
ZF = 0
SF = 0
PC = 3
```

예외:

```text
value(src) == 0 -> DivisionByZero
```

## 10. Logical Semantics

## 10.1 AND

형식:

```asm
AND dst, src
```

의미:

```text
result = value(dst) & value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

## 10.2 OR

형식:

```asm
OR dst, src
```

의미:

```text
result = value(dst) | value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

## 10.3 XOR

형식:

```asm
XOR dst, src
```

의미:

```text
result = value(dst) ^ value(src)
dst = result
Update ZF
Update SF
PC = PC + 1
```

## 10.4 NOT

형식:

```asm
NOT dst
```

의미:

```text
result = ~value(dst)
dst = result
Update ZF
Update SF
PC = PC + 1
```

## 11. Comparison Semantics

## 11.1 CMP

형식:

```asm
CMP lhs, rhs
```

의미:

```text
result = value(lhs) - value(rhs)
Update ZF
Update SF
PC = PC + 1
```

중요한 점은 `CMP`는 레지스터 값을 변경하지 않는다는 것이다.
오직 플래그만 변경한다.

예시:

```asm
CMP R1, R2
```

상태 전이:

```text
Before:
R1 = 10
R2 = 10
ZF = 0
SF = 0
PC = 2

After:
R1 = 10
R2 = 10
ZF = 1
SF = 0
PC = 3
```

## 11.2 TEST

형식:

```asm
TEST lhs, rhs
```

의미:

```text
result = value(lhs) & value(rhs)
Update ZF
Update SF
PC = PC + 1
```

`TEST` 역시 레지스터 값을 변경하지 않는다.

## 12. Branch Semantics

## 12.1 JMP

형식:

```asm
JMP label
```

의미:

```text
PC = label_address
```

## 12.2 JE

형식:

```asm
JE label
```

의미:

```text
if ZF == 1:
    PC = label_address
else:
    PC = PC + 1
```

## 12.3 JNE

형식:

```asm
JNE label
```

의미:

```text
if ZF == 0:
    PC = label_address
else:
    PC = PC + 1
```

## 12.4 JG

형식:

```asm
JG label
```

초기 단순 비교 규칙:

```text
if ZF == 0 and SF == 0:
    PC = label_address
else:
    PC = PC + 1
```

이 규칙은 `CMP lhs, rhs` 이후 `lhs > rhs`를 단순하게 표현하기 위한 것이다.
실제 CPU의 signed comparison과는 다를 수 있다.

## 12.5 JL

형식:

```asm
JL label
```

초기 단순 비교 규칙:

```text
if SF == 1:
    PC = label_address
else:
    PC = PC + 1
```

이 규칙은 `CMP lhs, rhs` 이후 `lhs < rhs`를 단순하게 표현하기 위한 것이다.

## 13. Stack Semantics

## 13.1 PUSH

형식:

```asm
PUSH src
```

의미:

```text
Memory[SP] = value(src)
SP = SP + 1
PC = PC + 1
```

예시:

```asm
PUSH R1
```

상태 전이:

```text
Before:
R1 = 10
SP = 256
Memory[256] = 0
PC = 2

After:
R1 = 10
SP = 257
Memory[256] = 10
PC = 3
```

## 13.2 POP

형식:

```asm
POP dst
```

의미:

```text
SP = SP - 1
dst = Memory[SP]
PC = PC + 1
```

예시:

```asm
POP R1
```

상태 전이:

```text
Before:
SP = 257
Memory[256] = 10
R1 = 0
PC = 3

After:
SP = 256
Memory[256] = 10
R1 = 10
PC = 4
```

## 14. Function Call Semantics

## 14.1 CALL

형식:

```asm
CALL label
```

의미:

```text
Memory[SP] = PC + 1
SP = SP + 1
PC = label_address
```

예시:

```asm
CALL function
```

상태 전이:

```text
Before:
PC = 0
SP = 256
Memory[256] = 0

After:
PC = label_address(function)
SP = 257
Memory[256] = 1
```

## 14.2 RET

형식:

```asm
RET
```

의미:

```text
SP = SP - 1
PC = Memory[SP]
```

상태 전이:

```text
Before:
SP = 257
Memory[256] = 1
PC = 5

After:
SP = 256
PC = 1
```

## 15. Control Semantics

## 15.1 NOP

형식:

```asm
NOP
```

의미:

```text
PC = PC + 1
```

## 15.2 HALT

형식:

```asm
HALT
```

의미:

```text
halted = true
```

`HALT` 이후 CPU는 더 이상 명령어를 실행하지 않는다.

## 16. Exception Semantics

Zero-CPU는 실행 중 다음 예외 상황을 처리한다.

| 예외                  | 조건                       |
| ------------------- | ------------------------ |
| `InvalidOpcode`     | 정의되지 않은 opcode 실행        |
| `InvalidOperand`    | 명령어 형식에 맞지 않는 operand 사용 |
| `InvalidRegister`   | 존재하지 않는 register 접근      |
| `MemoryOutOfBounds` | 메모리 범위를 벗어난 접근           |
| `StackOverflow`     | 스택 영역 초과                 |
| `StackUnderflow`    | 비어 있는 스택에서 POP 또는 RET    |
| `DivisionByZero`    | 0으로 나누기                  |
| `UnknownLabel`      | 존재하지 않는 라벨 참조            |

예외가 발생하면 CPU 실행을 중지하거나, 디버거가 해당 오류를 출력하도록 한다.

초기 구현에서는 다음 방식으로 처리한다.

```text
error = true
halted = true
emit error trace
```

## 17. Trace Event Semantics

Zero-CPU의 모든 명령어 실행은 하나의 트레이스 이벤트를 생성한다.

```text
TraceEvent = {
    pc_before,
    instruction,
    state_before,
    state_after,
    changed_registers,
    changed_memory,
    changed_flags,
    pc_after,
    error
}
```

예시:

```text
[PC=0002] ADD R1, R2

Before:
R1 = 10
R2 = 20
ZF = 0
SF = 0

After:
R1 = 30
R2 = 20
ZF = 0
SF = 0

Changed:
R1: 10 -> 30

NextPC:
0003
```

## 18. Determinism

Zero-CPU의 실행은 결정적이어야 한다.

즉, 같은 프로그램과 같은 초기 CPU 상태가 주어지면 항상 같은 최종 CPU 상태와 같은 트레이스 결과가 나와야 한다.

```text
same program + same initial state
= same final state + same trace
```

이 원칙은 테스트와 디버깅에서 중요하다.

## 19. Testing Strategy

각 명령어는 다음 기준으로 테스트한다.

1. 실행 전 상태를 설정한다.
2. 명령어 하나를 실행한다.
3. 실행 후 상태를 검증한다.
4. 변경되어야 하는 값만 변경되었는지 확인한다.
5. 변경되면 안 되는 값이 유지되었는지 확인한다.

예시 테스트:

```text
Test: ADD R1, R2

Initial:
R1 = 10
R2 = 20
PC = 0

Instruction:
ADD R1, R2

Expected:
R1 = 30
R2 = 20
PC = 1
ZF = 0
SF = 0
```

## 20. Summary

Zero-CPU의 실행 의미론은 명령어가 CPU 상태를 어떻게 변화시키는지 정의한다.

이 문서의 핵심은 다음과 같다.

* CPU 실행은 상태 전이로 표현된다.
* 모든 명령어는 실행 전후 상태를 가진다.
* 산술, 논리, 비교 명령어는 플래그를 변경한다.
* 분기 명령어는 PC를 직접 변경한다.
* 스택과 함수 호출은 SP와 Memory를 함께 변경한다.
* 모든 실행은 트레이스 이벤트로 기록된다.
* 같은 입력은 항상 같은 결과를 만들어야 한다.

Zero-CPU는 이 실행 의미론을 기준으로 구현되며, 이후 테스트 코드와 트레이스 시스템은 이 문서의 규칙을 검증하는 방향으로 작성한다.
