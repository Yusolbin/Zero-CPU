# CPU Step Trace Recording

Zero-CPU now records a `TraceEvent` for actual CPU execution steps.

This connects the v0.2 visual trace metadata to real execution.

## Source program execution

For source-level `.zasm` execution, `CPU::step()` now captures:

```text
before CPUState
current Instruction
after CPUState
error message, if any
```

Then it records:

```cpp
trace_logger_.record(TraceEvent(before, instruction, state_, error_message));
```

## Binary program execution

For `.zbin` execution, the CPU decodes the current binary instruction into a
trace-friendly `Instruction` before executing it.

This lets the existing `TraceEvent` system classify the step as:

```text
ALU_ADD
MEMORY_READ
MEMORY_WRITE
SOFTWARE_INTERRUPT
INTERRUPT_RETURN
CONDITIONAL_BRANCH
```

and expose a visual datapath such as:

```text
PC -> InstructionMemory -> Decoder -> RegisterFile -> ALU -> Flags -> Writeback
```

## Interrupt delivery

When a pending interrupt is serviced before normal instruction execution, the
CPU records a trace event using an `INT`-style pseudo instruction.

This is not yet a full hardware interrupt trace type, but it gives Studio a
visible event for interrupt delivery.

## Next step

Studio can now start reading `cpu.traceLogger().events()` and render the latest
event in a Visual Datapath Panel.
