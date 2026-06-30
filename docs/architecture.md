# Zero-CPU Architecture

## 1. Architecture Overview

Zero-CPU는 학습 및 분석 목적의 가상 CPU 아키텍처이다.
이 아키텍처는 실제 x86 또는 ARM을 그대로 모방하지 않고, CPU의 핵심 동작 원리를 이해하기 위해 필요한 구성 요소를 단순화하여 설계한다.

Zero-CPU의 핵심 목표는 다음과 같다.

* 명령어가 CPU 상태를 어떻게 변화시키는지 관찰한다.
* 레지스터, 메모리, 플래그, 스택, PC의 관계를 코드 수준에서 구현한다.
* Fetch-Decode-Execute 사이클을 직접 구현한다.
* 각 명령어 실행 전후의 상태 전이를 추적한다.
* 어셈블리 코드가 CPU 내부 명령어로 변환되고 실행되는 과정을 구현한다.

## 2. Core Components

Zero-CPU는 다음 구성 요소로 이루어진다.

```text
+----------------------+
|       Program        |
|      (.zasm)         |
+----------+-----------+
           |
           v
+----------------------+
|      Assembler       |
+----------+-----------+
           |
           v
+----------------------+
|    Instruction List  |
+----------+-----------+
           |
           v
+----------------------+
|         CPU          |
|                      |
|  +----------------+  |
|  | Register File  |  |
|  +----------------+  |
|  | Memory         |  |
|  +----------------+  |
|  | Flags          |  |
|  +----------------+  |
|  | PC / SP        |  |
|  +----------------+  |
|  | ALU            |  |
|  +----------------+  |
|  | Decoder        |  |
|  +----------------+  |
|  | Execution Unit |  |
|  +----------------+  |
+----------+-----------+
           |
           v
+----------------------+
|    Trace Logger      |
+----------------------+
```

## 3. CPU State

CPU의 상태는 특정 시점에서 CPU 내부에 저장된 모든 실행 정보를 의미한다.
Zero-CPU에서 CPU 상태는 다음 요소로 구성된다.

* General Purpose Registers
* Program Counter
* Stack Pointer
* Flags Register
* Memory
* Halt State

CPU 상태는 명령어가 실행될 때마다 변경될 수 있다.

## 4. Register File

Register File은 CPU가 연산에 사용할 값을 저장하는 공간이다.

초기 설계에서는 다음 범위의 범용 레지스터를 사용한다.

```text
R0
R1
R2
R3
R4
R5
R6
R7
```

각 레지스터는 정수 값을 저장한다.
초기 구현에서는 단순성을 위해 고정 크기 정수 타입을 사용할 수 있으며, 이후 확장 단계에서 64-bit 또는 320-bit 레지스터 구조로 확장할 수 있다.

### Register Usage

```text
R0 - General Purpose Register
R1 - General Purpose Register
R2 - General Purpose Register
R3 - General Purpose Register
R4 - General Purpose Register
R5 - General Purpose Register
R6 - General Purpose Register
R7 - General Purpose Register
```

## 5. Program Counter

Program Counter, 즉 PC는 다음에 실행할 명령어의 위치를 가리킨다.

CPU는 매 실행 사이클마다 PC가 가리키는 명령어를 가져온다.
일반 명령어가 실행되면 PC는 다음 명령어를 가리키도록 증가한다.

```text
PC = PC + 1
```

하지만 분기 명령어가 실행되면 PC는 특정 라벨 또는 주소로 변경된다.

```text
JMP target
PC = target
```

조건 분기 명령어의 경우 플래그 값에 따라 PC 변경 여부가 결정된다.

## 6. Stack Pointer

Stack Pointer, 즉 SP는 스택의 현재 위치를 가리킨다.

스택은 다음 기능을 위해 사용된다.

* PUSH
* POP
* CALL
* RET
* 함수 호출 복귀 주소 저장

초기 설계에서는 메모리의 일부 영역을 스택으로 사용한다.

예시:

```text
Memory[0 ... 255]      : General Memory
Memory[256 ... 511]    : Stack Area
```

스택은 높은 주소에서 낮은 주소로 자라거나, 낮은 주소에서 높은 주소로 자라도록 설계할 수 있다.
초기 구현에서는 단순성을 위해 낮은 주소에서 높은 주소로 증가하는 스택을 사용할 수 있다.

## 7. Flags Register

Flags Register는 연산 결과에 대한 상태 정보를 저장한다.

초기 구현 대상 플래그는 다음과 같다.

```text
ZF - Zero Flag
SF - Sign Flag
OF - Overflow Flag
CF - Carry Flag
```

### ZF: Zero Flag

연산 결과가 0이면 설정된다.

```text
result == 0 -> ZF = 1
result != 0 -> ZF = 0
```

### SF: Sign Flag

연산 결과가 음수이면 설정된다.

```text
result < 0 -> SF = 1
result >= 0 -> SF = 0
```

### OF: Overflow Flag

연산 결과가 표현 가능한 범위를 초과하면 설정된다.

초기 구현에서는 단순성을 위해 선택적으로 구현할 수 있다.

### CF: Carry Flag

부호 없는 연산에서 자리올림 또는 빌림이 발생하면 설정된다.

초기 구현에서는 선택적으로 구현할 수 있다.

## 8. Memory Model

Zero-CPU의 메모리는 단순한 선형 메모리 구조를 사용한다.

```text
Memory[0]
Memory[1]
Memory[2]
...
Memory[N-1]
```

초기 메모리 크기는 다음과 같이 설정할 수 있다.

```text
Memory Size = 1024 cells
```

각 메모리 셀은 정수 값을 저장한다.

메모리는 다음 명령어를 통해 접근한다.

```asm
LOAD R1, [100]
STORE [100], R1
```

### Memory Access

* `LOAD`: 메모리 값을 레지스터로 가져온다.
* `STORE`: 레지스터 값을 메모리에 저장한다.

## 9. Instruction Format

Zero-CPU의 내부 명령어는 다음 구조로 표현한다.

```cpp
struct Instruction {
    Opcode opcode;
    Operand dst;
    Operand src;
};
```

명령어는 크게 다음 요소로 구성된다.

* Opcode
* Destination Operand
* Source Operand
* Optional Immediate Value
* Optional Label

예시:

```asm
MOV R1, 10
ADD R1, R2
JMP loop
HALT
```

## 10. Operand Types

Zero-CPU의 피연산자는 다음과 같은 타입을 가진다.

```text
REGISTER
IMMEDIATE
MEMORY_ADDRESS
LABEL
NONE
```

### REGISTER

레지스터를 의미한다.

```asm
R1
R2
```

### IMMEDIATE

즉시값을 의미한다.

```asm
10
-5
```

### MEMORY_ADDRESS

메모리 주소를 의미한다.

```asm
[100]
```

### LABEL

분기 대상 라벨을 의미한다.

```asm
loop
end
```

## 11. Fetch-Decode-Execute Cycle

Zero-CPU는 다음 사이클을 반복한다.

```text
1. Fetch
2. Decode
3. Execute
4. Trace
5. Update PC
```

### 11.1 Fetch

PC가 가리키는 명령어를 가져온다.

```text
instruction = program[PC]
```

### 11.2 Decode

가져온 명령어의 Opcode와 Operand를 해석한다.

```text
opcode = instruction.opcode
dst = instruction.dst
src = instruction.src
```

### 11.3 Execute

명령어의 의미에 따라 CPU 상태를 변경한다.

예시:

```asm
ADD R1, R2
```

실행 의미:

```text
R1 = R1 + R2
Update Flags
```

### 11.4 Trace

명령어 실행 전후의 CPU 상태를 기록한다.

```text
Before State
Instruction
After State
Changed Registers
Changed Flags
Next PC
```

### 11.5 Update PC

일반 명령어는 PC를 1 증가시킨다.

```text
PC = PC + 1
```

분기 명령어는 PC를 분기 대상으로 변경한다.

```text
PC = target
```

## 12. Execution Example

다음과 같은 프로그램이 있다고 가정한다.

```asm
MOV R1, 10
MOV R2, 20
ADD R1, R2
HALT
```

초기 상태:

```text
R1 = 0
R2 = 0
PC = 0
ZF = 0
SF = 0
```

실행 흐름:

```text
PC=0 -> MOV R1, 10
PC=1 -> MOV R2, 20
PC=2 -> ADD R1, R2
PC=3 -> HALT
```

최종 상태:

```text
R1 = 30
R2 = 20
PC = 3
HALT = true
```

## 13. Trace Example

`ADD R1, R2` 명령어 실행 시 예상 트레이스는 다음과 같다.

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

## 14. Design Principles

Zero-CPU는 다음 설계 원칙을 따른다.

### 14.1 Simplicity First

처음부터 복잡한 실제 CPU 구조를 구현하지 않는다.
CPU의 핵심 개념을 이해할 수 있도록 단순하고 명확한 구조를 우선한다.

### 14.2 Traceable Execution

모든 명령어 실행은 추적 가능해야 한다.
실행 결과만 확인하는 것이 아니라, 실행 과정에서 어떤 상태가 변화했는지를 기록한다.

### 14.3 Testable Semantics

각 명령어는 테스트 가능한 실행 의미론을 가져야 한다.
예를 들어 `ADD`, `SUB`, `CMP`, `JMP`는 입력 상태와 출력 상태를 비교하여 검증할 수 있어야 한다.

### 14.4 Modular Design

CPU, ISA, Assembler, Debugger, Trace 시스템은 서로 분리된 모듈로 구성한다.

```text
core/
isa/
assembler/
debugger/
trace/
```

### 14.5 Documentation-Driven Development

구현 전에 문서로 구조와 명령어 의미를 먼저 정의한다.
이후 코드는 문서에서 정의한 구조를 기준으로 작성한다.

## 15. Future Expansion

초기 구현이 완료된 이후 다음 기능을 확장할 수 있다.

* 64-bit register mode
* 320-bit experimental register mode
* Interrupt simulation
* Simple process model
* Virtual memory model
* Pipeline simulation
* Cache simulation
* Web-based visualizer
* Compiler backend experiment

## 16. Summary

Zero-CPU는 가상 ISA, CPU 상태 모델, 명령어 실행 엔진, 어셈블러, 디버거, 트레이스 시스템으로 구성된 C++ 기반 CPU 에뮬레이터이다.

이 아키텍처의 핵심은 명령어 실행을 단순히 처리하는 것이 아니라, 실행 전후의 CPU 상태 변화를 추적하고 분석하는 것이다.

이를 통해 Zero-CPU는 컴퓨터 구조와 시스템 프로그래밍을 직접 구현하며 학습할 수 있는 연구형 포트폴리오 프로젝트로 발전할 수 있다.
