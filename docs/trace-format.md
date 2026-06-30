# Zero-CPU Trace Format

## 1. Overview

이 문서는 Zero-CPU의 실행 트레이스 형식을 정의한다.

Zero-CPU의 핵심 목표는 단순히 명령어를 실행하는 것이 아니라, 각 명령어가 실행될 때 CPU 내부 상태가 어떻게 변화하는지를 추적하는 것이다.

따라서 Zero-CPU는 모든 명령어 실행에 대해 다음 정보를 기록한다.

* 실행 전 Program Counter
* 실행된 명령어
* 실행 전 CPU 상태
* 실행 후 CPU 상태
* 변경된 레지스터
* 변경된 플래그
* 변경된 메모리
* 변경된 Stack Pointer
* 실행 후 Program Counter
* 예외 발생 여부

이 트레이스는 디버깅, 테스트, 실행 의미론 검증, 학습용 분석에 사용된다.

## 2. Trace Event

Zero-CPU에서 하나의 명령어 실행은 하나의 Trace Event로 기록된다.

```text
TraceEvent = {
    pc_before,
    instruction,
    state_before,
    state_after,
    changed_registers,
    changed_flags,
    changed_memory,
    sp_before,
    sp_after,
    pc_after,
    error
}
```

각 Trace Event는 하나의 상태 전이를 나타낸다.

```text
State_before -- instruction --> State_after
```

## 3. Trace Event Fields

| 필드                  | 설명              |
| ------------------- | --------------- |
| `pc_before`         | 명령어 실행 전 PC 값   |
| `instruction`       | 실행된 명령어         |
| `state_before`      | 명령어 실행 전 CPU 상태 |
| `state_after`       | 명령어 실행 후 CPU 상태 |
| `changed_registers` | 변경된 레지스터 목록     |
| `changed_flags`     | 변경된 플래그 목록      |
| `changed_memory`    | 변경된 메모리 주소와 값   |
| `sp_before`         | 명령어 실행 전 SP 값   |
| `sp_after`          | 명령어 실행 후 SP 값   |
| `pc_after`          | 명령어 실행 후 PC 값   |
| `error`             | 예외 발생 정보        |

## 4. Human-Readable Trace Format

초기 구현에서는 사람이 읽기 쉬운 텍스트 기반 트레이스 형식을 우선 사용한다.

예시:

```text
[PC=0002] ADD R1, R2

Before:
  R1 = 10
  R2 = 20
  ZF = 0
  SF = 0
  SP = 256

After:
  R1 = 30
  R2 = 20
  ZF = 0
  SF = 0
  SP = 256

Changed Registers:
  R1: 10 -> 30

Changed Flags:
  None

Changed Memory:
  None

NextPC:
  0003

Error:
  None
```

이 형식은 CLI 디버거와 개발 중 로그 확인에 사용한다.

## 5. Compact Trace Format

터미널에서 여러 명령어 실행 결과를 빠르게 확인하기 위해 간단한 한 줄 형식도 사용할 수 있다.

```text
PC=0002 | ADD R1, R2 | R1:10->30 | ZF:0 | SF:0 | NextPC=0003
```

예시:

```text
PC=0000 | MOV R1, 10 | R1:0->10 | NextPC=0001
PC=0001 | MOV R2, 20 | R2:0->20 | NextPC=0002
PC=0002 | ADD R1, R2 | R1:10->30 | ZF:0 | SF:0 | NextPC=0003
PC=0003 | HALT | halted:false->true
```

Compact Trace는 CLI에서 `trace` 명령어를 실행했을 때 간단히 보여주는 용도로 사용할 수 있다.

## 6. JSON-like Trace Format

이후 테스트 자동화 또는 외부 시각화 도구와 연동하기 위해 JSON과 유사한 구조화된 형식을 사용할 수 있다.

초기 버전에서는 실제 JSON 라이브러리를 사용하지 않더라도, 내부 구조는 다음과 같이 설계한다.

```json
{
  "pc_before": 2,
  "instruction": "ADD R1, R2",
  "before": {
    "registers": {
      "R1": 10,
      "R2": 20
    },
    "flags": {
      "ZF": false,
      "SF": false
    },
    "sp": 256,
    "halted": false
  },
  "after": {
    "registers": {
      "R1": 30,
      "R2": 20
    },
    "flags": {
      "ZF": false,
      "SF": false
    },
    "sp": 256,
    "halted": false
  },
  "changed_registers": [
    {
      "name": "R1",
      "before": 10,
      "after": 30
    }
  ],
  "changed_flags": [],
  "changed_memory": [],
  "pc_after": 3,
  "error": null
}
```

이 형식은 이후 웹 기반 시각화 도구나 테스트 리포트 생성에 사용할 수 있다.

## 7. Register Change Format

레지스터 값이 변경되었을 경우 다음 형식으로 기록한다.

```text
R1: 10 -> 30
```

구조화된 표현은 다음과 같다.

```text
RegisterChange = {
    name,
    before,
    after
}
```

예시:

```text
{
    name: "R1",
    before: 10,
    after: 30
}
```

레지스터 값이 변경되지 않은 경우에는 기록하지 않는다.

## 8. Flag Change Format

플래그 값이 변경되었을 경우 다음 형식으로 기록한다.

```text
ZF: 0 -> 1
```

구조화된 표현은 다음과 같다.

```text
FlagChange = {
    name,
    before,
    after
}
```

예시:

```text
{
    name: "ZF",
    before: false,
    after: true
}
```

값이 변경되지 않은 플래그는 `Changed Flags` 목록에 포함하지 않는다.

단, 사람이 읽는 Trace 출력에서는 현재 플래그 상태를 `Before`, `After` 영역에 표시할 수 있다.

## 9. Memory Change Format

메모리 값이 변경되었을 경우 다음 형식으로 기록한다.

```text
Memory[100]: 0 -> 77
```

구조화된 표현은 다음과 같다.

```text
MemoryChange = {
    address,
    before,
    after
}
```

예시:

```text
{
    address: 100,
    before: 0,
    after: 77
}
```

메모리 변경은 주로 다음 명령어에서 발생한다.

* `STORE`
* `PUSH`
* `CALL`

## 10. Stack Change Format

스택은 메모리와 SP를 함께 사용하므로, 스택 관련 명령어는 `SP` 변화와 `Memory` 변화를 함께 기록한다.

예시:

```asm
PUSH R1
```

실행 전:

```text
R1 = 10
SP = 256
Memory[256] = 0
```

실행 후:

```text
R1 = 10
SP = 257
Memory[256] = 10
```

Trace 예시:

```text
[PC=0002] PUSH R1

Before:
  R1 = 10
  SP = 256
  Memory[256] = 0

After:
  R1 = 10
  SP = 257
  Memory[256] = 10

Changed Memory:
  Memory[256]: 0 -> 10

Changed Stack Pointer:
  SP: 256 -> 257

NextPC:
  0003
```

## 11. PC Change Format

모든 명령어는 실행 후 `PC`에 영향을 줄 수 있다.

일반 명령어는 다음과 같이 기록한다.

```text
PC: 0002 -> 0003
```

분기 명령어는 다음과 같이 기록한다.

```text
PC: 0004 -> 0010
Branch Taken: true
```

조건 분기가 실패한 경우:

```text
PC: 0004 -> 0005
Branch Taken: false
```

## 12. Branch Trace Format

분기 명령어는 조건 판단 결과를 함께 기록한다.

예시:

```asm
CMP R1, R2
JE equal
```

`JE equal` 실행 Trace:

```text
[PC=0003] JE equal

Before:
  ZF = 1
  PC = 3

Branch:
  Condition: ZF == 1
  Result: true
  Target: equal
  TargetAddress: 6

After:
  PC = 6

NextPC:
  0006
```

조건이 거짓인 경우:

```text
[PC=0003] JE equal

Before:
  ZF = 0
  PC = 3

Branch:
  Condition: ZF == 1
  Result: false
  Target: equal
  TargetAddress: 6

After:
  PC = 4

NextPC:
  0004
```

## 13. Function Call Trace Format

`CALL` 명령어는 복귀 주소를 스택에 저장하고 함수 라벨로 이동한다.

예시:

```asm
CALL set_value
```

Trace:

```text
[PC=0000] CALL set_value

Before:
  PC = 0
  SP = 256
  Memory[256] = 0

Call:
  ReturnAddress: 1
  Target: set_value
  TargetAddress: 4

After:
  PC = 4
  SP = 257
  Memory[256] = 1

Changed Memory:
  Memory[256]: 0 -> 1

Changed Stack Pointer:
  SP: 256 -> 257

NextPC:
  0004
```

## 14. Return Trace Format

`RET` 명령어는 스택에서 복귀 주소를 꺼내 PC에 저장한다.

예시:

```asm
RET
```

Trace:

```text
[PC=0005] RET

Before:
  PC = 5
  SP = 257
  Memory[256] = 1

Return:
  ReturnAddress: 1

After:
  PC = 1
  SP = 256

Changed Stack Pointer:
  SP: 257 -> 256

NextPC:
  0001
```

## 15. Error Trace Format

명령어 실행 중 예외가 발생하면 Trace Event에 오류 정보를 포함한다.

예시:

```asm
DIV R1, R2
```

실행 전:

```text
R1 = 10
R2 = 0
```

Trace:

```text
[PC=0002] DIV R1, R2

Before:
  R1 = 10
  R2 = 0
  PC = 2

Error:
  Type: DivisionByZero
  Message: Cannot divide by zero.

After:
  halted = true
  error = true

NextPC:
  0002
```

오류 발생 시 초기 구현에서는 CPU를 중지한다.

```text
halted = true
error = true
```

## 16. Trace Output Modes

Zero-CPU는 다음 Trace 출력 모드를 지원할 수 있다.

| 모드        | 설명                      |
| --------- | ----------------------- |
| `none`    | Trace를 출력하지 않음          |
| `compact` | 한 줄 요약 Trace 출력         |
| `full`    | 상세 Trace 출력             |
| `json`    | 구조화된 JSON-like Trace 출력 |

초기 구현에서는 `compact`, `full` 모드를 우선 구현한다.

```text
trace compact
trace full
```

## 17. Trace Storage

Trace는 다음 방식으로 저장할 수 있다.

### 17.1 In-Memory Trace

실행 중 발생한 Trace Event를 메모리의 리스트에 저장한다.

```text
vector<TraceEvent> traces;
```

장점:

* 디버거에서 바로 확인 가능
* 테스트 코드에서 검증하기 쉬움

단점:

* 실행 명령어 수가 많아지면 메모리 사용량 증가

### 17.2 File Trace

Trace Event를 파일로 저장한다.

예상 파일명:

```text
trace.log
```

예시:

```text
zero_cpu --trace examples/simple_add.zasm
```

출력:

```text
trace.log
```

파일 저장은 이후 확장 기능으로 구현할 수 있다.

## 18. Trace and Debugger

CLI 디버거는 Trace 시스템과 연결된다.

예상 명령어:

```text
trace
trace compact
trace full
last
```

명령어 설명:

| 명령어             | 설명                 |
| --------------- | ------------------ |
| `trace`         | 현재까지의 Trace 출력     |
| `trace compact` | 간단한 Trace 출력       |
| `trace full`    | 상세 Trace 출력        |
| `last`          | 마지막 Trace Event 출력 |

예시:

```text
zero-cpu> step
PC=0000 | MOV R1, 10 | R1:0->10 | NextPC=0001

zero-cpu> step
PC=0001 | MOV R2, 20 | R2:0->20 | NextPC=0002

zero-cpu> step
PC=0002 | ADD R1, R2 | R1:10->30 | NextPC=0003

zero-cpu> trace
PC=0000 | MOV R1, 10 | R1:0->10 | NextPC=0001
PC=0001 | MOV R2, 20 | R2:0->20 | NextPC=0002
PC=0002 | ADD R1, R2 | R1:10->30 | NextPC=0003
```

## 19. Trace and Testing

Trace는 테스트 검증에도 사용된다.

예를 들어 `ADD R1, R2` 테스트에서는 다음을 검증할 수 있다.

```text
Initial:
  R1 = 10
  R2 = 20

Instruction:
  ADD R1, R2

Expected Trace:
  changed_registers contains R1: 10 -> 30
  pc_before = 0
  pc_after = 1
  error = null
```

테스트 기준:

* 변경되어야 하는 레지스터가 정확히 변경되었는가?
* 변경되면 안 되는 레지스터가 유지되었는가?
* 플래그가 올바르게 변경되었는가?
* PC가 올바르게 이동했는가?
* 예외 발생 여부가 올바른가?

## 20. Trace Example: Simple Addition

프로그램:

```asm
MOV R1, 10
MOV R2, 20
ADD R1, R2
HALT
```

Compact Trace:

```text
PC=0000 | MOV R1, 10 | R1:0->10 | NextPC=0001
PC=0001 | MOV R2, 20 | R2:0->20 | NextPC=0002
PC=0002 | ADD R1, R2 | R1:10->30 | ZF:0 | SF:0 | NextPC=0003
PC=0003 | HALT | halted:false->true
```

최종 상태:

```text
R1 = 30
R2 = 20
halted = true
```

## 21. Design Principles

Zero-CPU의 Trace Format은 다음 원칙을 따른다.

### 21.1 Readable

Trace는 사람이 읽을 수 있어야 한다.
디버거에서 바로 확인할 수 있는 텍스트 형식을 우선한다.

### 21.2 Comparable

실행 전후 상태를 비교할 수 있어야 한다.
변경된 값은 `before -> after` 형식으로 표현한다.

### 21.3 Testable

Trace는 테스트 검증에 사용할 수 있어야 한다.
각 Trace Event는 명령어 하나의 실행 결과를 검증할 수 있는 정보를 포함한다.

### 21.4 Extensible

초기에는 텍스트 기반 Trace를 사용하지만, 이후 JSON 또는 웹 시각화 형식으로 확장할 수 있어야 한다.

## 22. Summary

Zero-CPU의 Trace Format은 명령어 실행 전후의 CPU 상태 변화를 기록하기 위한 형식이다.

이 문서에서 정의한 Trace Event는 다음 정보를 포함한다.

* 실행 전 PC
* 실행 명령어
* 실행 전 상태
* 실행 후 상태
* 변경된 레지스터
* 변경된 플래그
* 변경된 메모리
* SP 변화
* PC 변화
* 예외 정보

Trace 시스템은 Zero-CPU를 단순한 CPU 에뮬레이터가 아니라, 명령어 실행 의미론을 분석하고 검증할 수 있는 연구형 시스템 프로그래밍 프로젝트로 만드는 핵심 요소이다.
