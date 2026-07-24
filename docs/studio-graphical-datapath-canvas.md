# Studio Graphical Datapath Canvas

Zero-CPU Studio now has a graphical datapath canvas.

The previous Visual Datapath Panel showed text derived from the latest
`TraceEvent`. This canvas draws a stronger CPU block diagram using Win32 GDI.

## Blocks

The first canvas renders these conceptual CPU nodes:

```text
PC
InstructionMemory
Decoder
RegisterFile
ALU
Flags
Writeback
Memory/MMIO
Stack
InterruptController
```

## Highlight rule

The canvas reads:

```cpp
g_cpu.traceLogger().events().back().datapathNodes()
```

and highlights any block that appears in the latest trace event.

For example:

```asm
ADD R1, R2
```

can highlight:

```text
PC -> InstructionMemory -> Decoder -> RegisterFile -> ALU -> Flags -> Writeback
```

while a memory instruction can highlight:

```text
AddressUnit -> Memory/MMIO -> RegisterFile
```

## Why this matters

This is the first step where Studio stops being only a text debugger and starts
looking like a visual CPU datapath debugger.

The implementation is intentionally simple Win32 GDI so the project remains
self-contained and does not depend on a GUI framework.
