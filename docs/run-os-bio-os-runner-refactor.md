# `run-os` BioOSRunner Refactor

`zero_cli run-os` now uses the shared `BioOSRunner` module.

## Before

```text
zero_cli run-os
    had its own BIO-OS combine / assemble / load / execute flow
```

## After

```text
zero_cli run-os
    ??BioOSRunner
    ??BIO-OS result
```

The command name remains the same:

```bat
.\build\Debug\zero_cli.exe run-os examples\bio_os
```

Expected signs:

```text
BIO-OS run finished successfully.
ASCII view:
BU
Timer interrupt count = 1
Timer enabled = false
```

## Why

This removes duplication between the standalone BioOSRunner test path and the existing CLI OS runner path.

Next refactor target:

```text
Studio Run BIO-OS ??BioOSRunner
```