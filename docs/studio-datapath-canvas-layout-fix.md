# Studio Datapath Canvas Layout Fix

The first graphical datapath canvas was too short.

The canvas tried to draw:

```text
title
main datapath boxes
lower Memory/MMIO, Stack, InterruptCtl boxes
footer
```

inside a 120px-high child window. The lower row and footer overlapped, making
the canvas look duplicated or clipped.

## Fix

The canvas height is increased from 120px to 220px.

The scrollable content height is also increased so the main window scrollbar can
reach the full canvas area.
