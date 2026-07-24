# Studio Compact Datapath Canvas

The previous graphical datapath canvas could be clipped on smaller screens and
the footer could visually overlap with the lower node row.

This update turns the datapath canvas into a compact layout.

## Changes

```text
- canvas height reduced to a stable 150px
- title row remains at the top
- main datapath blocks are compressed
- Memory/MMIO, Stack, and InterruptCtl fit in the same canvas
- footer text is replaced with a small hint line
```

## Reason

The Studio UI should remain usable on normal laptop and desktop monitors while
still showing a strong visual CPU datapath.

The compact canvas is easier to see, screenshot, and use for portfolio demos.
