# Studio Visual Datapath Panel

Zero-CPU Studio now has a first visual datapath panel.

This is not the final graphical block-and-arrow view yet. It is the bridge
between structured CPU trace data and a stronger visual debugger.

## What it shows

The panel reads the latest `TraceEvent` from:

```cpp
g_cpu.traceLogger().events()
```

and displays:

```text
- instruction
- PC before / after
- stage
- action
- datapath path
- datapath nodes
- changed registers
- changed flags
- changed memory
```

## Example

For an ALU instruction such as:

```asm
ADD R1, R2
```

the panel can show:

```text
Stage = EXECUTE
Action = ALU_ADD
Path = PC -> InstructionMemory -> Decoder -> RegisterFile -> ALU -> Flags -> Writeback
```

## Why this matters

The Studio debugger is moving from plain state dumps toward a real CPU
datapath debugger.

The current panel is intentionally text-first so that the execution data can be
validated before building a graphical renderer.

## Next step

The next step is to turn the datapath nodes into a stronger graphical panel:

```text
PC --> InstructionMemory --> Decoder --> RegisterFile --> ALU --> Writeback
```

The active nodes from the latest `TraceEvent` can then be highlighted.
