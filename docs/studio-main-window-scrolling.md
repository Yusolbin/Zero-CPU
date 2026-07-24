# Studio Main Window Scrolling

Zero-CPU Studio can now scroll its main content area.

The graphical datapath canvas made the Studio window taller than some monitor
work areas. This patch keeps the richer layout while making the main window
usable on smaller screens.

## What changed

The Studio main window now uses:

```cpp
WS_OVERLAPPEDWINDOW | WS_VSCROLL
```

and handles:

```text
WM_SIZE
WM_VSCROLL
WM_MOUSEWHEEL
```

The implementation uses `ScrollWindowEx` with `SW_SCROLLCHILDREN`, so the child
controls move together when the main window scrolls.

## Why this matters

The Studio UI can now keep growing with more debugger panels without requiring
a huge monitor.

This is especially useful for:

```text
- Visual Datapath Panel
- Graphical Datapath Canvas
- future timeline views
- future memory map views
```
