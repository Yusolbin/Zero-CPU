# Zero-CPU

**Zero-CPU**는 직접 설계한 가상 ISA를 기반으로 동작하는 C++ 기반 CPU 에뮬레이터 프로젝트입니다.
이 프로젝트는 단순히 명령어를 실행하는 가상 머신을 만드는 것을 넘어, CPU 내부 상태가 명령어 실행에 따라 어떻게 변화하는지를 추적하고 분석하는 것을 목표로 합니다.

## Project Title

**Zero-CPU: 상태 전이 추적을 지원하는 가상 ISA 기반 CPU 에뮬레이터 구현**

## Overview

Zero-CPU는 저수준 프로그래밍과 컴퓨터 구조 학습을 목적으로 설계된 가상 CPU입니다.
레지스터, 메모리, 플래그, 프로그램 카운터, 스택 포인터, 명령어 디코더, 실행 엔진을 직접 구현하며, 자체 어셈블리 문법인 `.zasm`을 통해 프로그램을 작성하고 실행할 수 있도록 설계합니다.

이 프로젝트의 핵심은 **명령어 실행 전후의 CPU 상태를 추적하는 것**입니다.
각 명령어가 실행될 때 레지스터, 플래그, 메모리, PC, SP가 어떻게 변화하는지 기록하여 CPU의 동작 과정을 분석 가능한 형태로 남깁니다.

## Motivation

운영체제, 컴파일러, 가상 머신, 리버싱, 임베디드 시스템과 같은 저수준 시스템 분야를 이해하기 위해서는 CPU가 명령어를 어떻게 해석하고 실행하는지에 대한 이해가 필요합니다.

Zero-CPU는 실제 x86 또는 ARM 아키텍처를 그대로 구현하는 것이 아니라, 학습과 분석에 적합한 작은 가상 ISA를 직접 설계하고 구현함으로써 CPU의 기본 동작 원리를 코드 수준에서 이해하는 것을 목표로 합니다.

## Goals

이 프로젝트의 주요 목표는 다음과 같습니다.

* 자체 가상 ISA 설계
* C++ 기반 CPU 에뮬레이터 구현
* Fetch-Decode-Execute 사이클 구현
* 레지스터, 메모리, 플래그, 스택 구조 구현
* `.zasm` 어셈블리 파일 파싱
* CLI 기반 디버거 구현
* 명령어 실행 전후 상태 추적
* 테스트 가능한 CPU 실행 모델 구성
* GitHub 포트폴리오용 기술 문서 작성

## Core Features

### 1. Virtual CPU

Zero-CPU는 다음과 같은 CPU 구성 요소를 가집니다.

* Register File
* Program Counter
* Stack Pointer
* Flags Register
* Memory
* ALU
* Instruction Decoder
* Execution Engine

CPU는 기본적으로 다음 실행 흐름을 따릅니다.

```text
Fetch -> Decode -> Execute -> Update State
```

### 2. Virtual ISA

Zero-CPU는 자체 명령어 집합을 사용합니다.

초기 구현 대상 명령어는 다음과 같습니다.

```asm
MOV
LOAD
STORE

ADD
SUB
MUL
DIV

AND
OR
XOR
NOT

CMP
TEST

JMP
JE
JNE
JG
JL

PUSH
POP
CALL
RET

NOP
HALT
```

### 3. Assembler

Zero-CPU는 `.zasm` 형식의 어셈블리 파일을 읽어 내부 명령어 구조로 변환합니다.

예시:

```asm
MOV R1, 10
MOV R2, 20
ADD R1, R2
HALT
```

### 4. CLI Debugger

터미널에서 CPU 실행 흐름을 직접 제어할 수 있는 CLI 디버거를 구현합니다.

목표 명령어:

```text
run
step
regs
mem
stack
break
continue
trace
quit
```

### 5. Trace System

Zero-CPU의 가장 중요한 기능은 명령어 실행 전후의 상태를 기록하는 트레이스 시스템입니다.

예상 출력 예시:

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

NextPC = 0003
```

이를 통해 각 명령어가 CPU 상태에 어떤 영향을 주는지 확인할 수 있습니다.

## Directory Structure

```text
Zero-CPU/
│
├─ CMakeLists.txt
├─ README.md
│
├─ docs/
│  ├─ architecture.md
│  ├─ isa.md
│  ├─ execution-semantics.md
│  └─ trace-format.md
│
├─ examples/
│  ├─ simple_add.zasm
│  ├─ branch_test.zasm
│  └─ function_call.zasm
│
├─ include/
│  └─ zero_cpu/
│     ├─ core/
│     │  ├─ CPU.hpp
│     │  ├─ CPUState.hpp
│     │  ├─ RegisterFile.hpp
│     │  ├─ Memory.hpp
│     │  └─ Flags.hpp
│     │
│     ├─ isa/
│     │  ├─ Instruction.hpp
│     │  ├─ Opcode.hpp
│     │  └─ Operand.hpp
│     │
│     ├─ assembler/
│     │  ├─ Lexer.hpp
│     │  ├─ Parser.hpp
│     │  └─ Assembler.hpp
│     │
│     ├─ debugger/
│     │  └─ Debugger.hpp
│     │
│     └─ trace/
│        ├─ TraceEvent.hpp
│        └─ TraceLogger.hpp
│
├─ src/
│  ├─ core/
│  ├─ isa/
│  ├─ assembler/
│  ├─ debugger/
│  └─ trace/
│
├─ tests/
│  ├─ test_arithmetic.cpp
│  ├─ test_branch.cpp
│  ├─ test_stack.cpp
│  └─ test_memory.cpp
│
└─ tools/
   └─ zero_cli.cpp
```

## Development Roadmap

### Phase 1. Project Setup

* GitHub 저장소 생성
* CMake 프로젝트 구성
* 기본 디렉터리 구조 생성
* README 작성
* architecture.md 작성

### Phase 2. CPU State Model

* RegisterFile 구현
* Memory 구현
* Flags 구현
* CPUState 구현
* PC, SP 구조 정의

### Phase 3. Instruction Model

* Opcode 정의
* Operand 정의
* Instruction 구조체 구현
* 명령어 문자열 출력 기능 구현

### Phase 4. Execution Engine

* CPU 클래스 구현
* Fetch 단계 구현
* Decode 단계 구현
* Execute 단계 구현
* `NOP`, `HALT`, `MOV`, `ADD`, `SUB` 구현

### Phase 5. ISA Expansion

* 산술 연산 명령어 구현
* 논리 연산 명령어 구현
* 비교 명령어 구현
* 조건 분기 명령어 구현
* 스택 명령어 구현
* 함수 호출 명령어 구현

### Phase 6. Assembler

* `.zasm` 파일 입력 처리
* 토큰 분리
* 명령어 파싱
* 라벨 처리
* 내부 Instruction 형태로 변환

### Phase 7. Debugger

* step 실행
* run 실행
* register dump
* memory dump
* stack dump
* breakpoint 기능
* trace 출력 기능

### Phase 8. Trace System

* 실행 전 CPU 상태 기록
* 실행 후 CPU 상태 기록
* 변경된 레지스터 표시
* 변경된 플래그 표시
* 메모리 접근 기록
* 로그 파일 저장

### Phase 9. Testing

* 명령어 단위 테스트
* 분기 명령어 테스트
* 스택 명령어 테스트
* 함수 호출 테스트
* 메모리 load/store 테스트

## Expected Result

Zero-CPU의 최종 산출물은 다음과 같습니다.

* C++ 기반 가상 CPU 에뮬레이터
* 자체 ISA 문서
* `.zasm` 어셈블리 예제 파일
* 어셈블러
* CLI 디버거
* 트레이스 로그 시스템
* 테스트 코드
* GitHub 포트폴리오 문서
* 개발 회고 및 기술 블로그 글

## Long-Term Expansion

Zero-CPU는 이후 다음 방향으로 확장할 수 있습니다.

* 간단한 운영체제 커널 시뮬레이션
* 인터럽트 모델 추가
* 메모리 보호 모델 추가
* 프로세스/스레드 스케줄링 시뮬레이션
* 파이프라인 구조 실험
* 캐시 메모리 시뮬레이션
* 웹 기반 실행 시각화 도구
* 간단한 컴파일러 백엔드 연결

## Final Objective

Zero-CPU는 직접 설계한 가상 ISA를 기반으로 CPU의 명령어 실행 과정과 상태 전이를 추적할 수 있는 C++ 기반 CPU 에뮬레이터입니다.

이 프로젝트는 컴퓨터 구조, 시스템 프로그래밍, 어셈블러, 디버거, 실행 의미론을 직접 구현함으로써 저수준 시스템 프로그래밍 역량을 증명하는 것을 목표로 합니다.
