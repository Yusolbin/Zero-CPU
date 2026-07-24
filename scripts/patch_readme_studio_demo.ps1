param(
    [string]$RepoRoot = (Get-Location).Path
)

$ErrorActionPreference = "Stop"

$readmePath = Join-Path $RepoRoot "README.md"

if (-not (Test-Path $readmePath)) {
    throw "README.md not found. Run this script from the Zero-CPU repository root."
}

$text = Get-Content -Raw -Path $readmePath
$text = $text -replace "`r`n", "`n"

$backupPath = "$readmePath.bak_studio_demo"

if (-not (Test-Path $backupPath)) {
    Copy-Item $readmePath $backupPath
}

$section = @'

---

## Studio BIO-OS Demo

Zero-CPU Studio can run the current BIO-OS demo directly.

The Studio BIO-OS runner combines:

```text
examples\bio_os\boot.zasm
examples\bio_os\kernel.zasm
examples\bio_os\user_program.zasm
```

into generated files:

```text
examples\bio_os\combined_boot.zasm
examples\bio_os\combined_boot.zbin
```

Then it loads the generated `.zbin`, attaches the system devices, registers interrupt handlers, and runs the BIO-OS program.

```text
boot.zasm
    ↓
kernel.zasm
    ↓
user_program.zasm
    ↓
combined_boot.zasm
    ↓
combined_boot.zbin
    ↓
Zero-CPU Studio
    ↓
Run BIO-OS
```

### How to run in Studio

Build the project:

```bat
cd /d D:\Zero-CPU
cmake --build build
```

Open Studio:

```bat
.\build\Debug\zero_studio.exe
```

Click:

```text
Run BIO-OS
```

Expected result in the Studio trace panel:

```text
[Studio BIO-OS Run]
BIO-OS execution finished successfully.
Debug Writes: 2
Debug ASCII: BU
Timer Interrupt Count: 1
Timer Enabled: false
```

Expected result in the Studio system panel:

```text
Mode: Binary
Halted = true
Debug MMIO = 0xf000..0xf00f
Timer MMIO = 0xf100..0xf12f
Syscall Vector = 80
Default Timer Vector = 44
Timer Interval = 8
Timer Vector = 44
Timer Payload = 888
Timer Interrupt Count = 1
Timer Enabled = false
Debug Writes = 2
Debug ASCII = BU
```

This demonstrates that Studio is not only a source editor. It can inspect the virtual computer state while running a mini-kernel style BIO-OS demo.

---

## CLI BIO-OS Demo

The same BIO-OS demo can also be run from the command line:

```bat
.\build\Debug\zero_cli.exe run-os examples\bio_os
```

Expected output includes:

```text
BIO-OS run finished successfully.
ASCII view:
BU
Timer interrupt count = 1
Timer enabled = false
```

The CLI and Studio BIO-OS demos exercise the same core concepts:

```text
- .zasm assembly source
- assembler
- .zbin binary generation
- binary loader
- virtual memory
- CPU fetch/decode/execute
- INT 80 syscall handler
- MMIO debug output
- timer MMIO device
- hardware-style interrupt
- IRET return flow
- mini-kernel style dispatch
```

'@

if ($text.Contains("## Studio BIO-OS Demo")) {
    Write-Host "README already contains Studio BIO-OS Demo section. No changes made."
} else {
    $inserted = $false

    $anchors = @(
        "## Repository Structure",
        "## Core CPU Features",
        "## CLI Usage",
        "## Test Commands",
        "## Example Programs"
    )

    foreach ($anchor in $anchors) {
        if ($text.Contains($anchor)) {
            $text = $text.Replace($anchor, $section + "`n" + $anchor)
            $inserted = $true
            break
        }
    }

    if (-not $inserted) {
        $text = $text + $section
    }

    Set-Content -Path $readmePath -Value ($text -replace "`n", "`r`n") -NoNewline
    Write-Host "Patched README.md with Studio BIO-OS demo section."
}

Write-Host "Backup: $backupPath"
