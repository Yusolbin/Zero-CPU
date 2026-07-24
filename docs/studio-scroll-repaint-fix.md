# Studio Scroll Repaint Fix

The main-window scrollbar worked, but the graphical datapath canvas could leave
visual remnants while scrolling.

## Problem

`ScrollWindowEx` moved child windows, but the invalidated parent area was not
always fully erased and repainted immediately.

This made the datapath canvas appear duplicated near the bottom of the window.

## Fix

The scroll handler now uses:

```cpp
SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE
```

and forces a full redraw with:

```cpp
RedrawWindow(
    hwnd,
    nullptr,
    nullptr,
    RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | RDW_UPDATENOW
);
```

The main window also uses `WS_CLIPCHILDREN`, and the datapath canvas uses
`WS_CLIPSIBLINGS` to reduce child-window painting artifacts.

## Result

The datapath canvas should no longer leave duplicated ghost drawings when the
main Studio window scrolls.
