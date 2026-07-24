# Visual Trace Event Model

Zero-CPU v0.2 starts with a stronger trace model for visual debugging.

The existing `TraceEvent` already tracked state differences:

```text
- PC before / after
- changed registers
- changed flags
- changed memory
- error message
```

This adds visual metadata:

```text
- stage
- action
- datapath nodes
```

## Example

For:

```asm
ADD R1, R2
```

A trace event can now expose:

```text
stage  = EXECUTE
action = ALU_ADD
path   = PC -> InstructionMemory -> Decoder -> RegisterFile -> ALU -> Flags -> Writeback
```

## Why this matters

Studio should not draw from free-form text logs.

Studio should draw from structured execution events.

The target v0.2 direction is:

```text
Instruction
    ↓
TraceEvent
    ↓
Visual Datapath Panel
    ↓
Register / ALU / Flags / Memory / Interrupt visualization
```

## Next step

The next patch should make real CPU stepping record trace events consistently for both:

```text
- source-level .zasm execution
- binary .zbin execution
```

Then Studio can consume those events.
