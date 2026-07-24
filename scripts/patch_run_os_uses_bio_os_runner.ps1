param(
    [string]$RepoRoot = (Get-Location).Path
)

$ErrorActionPreference = "Stop"

Write-Host "START patch_run_os_uses_bio_os_runner"

function Normalize-Newlines {
    param([string]$Text)
    return $Text -replace "`r`n", "`n"
}

function Save-Utf8NoBom {
    param(
        [string]$Path,
        [string]$Text
    )

    $dir = Split-Path -Parent $Path
    if ($dir -and -not (Test-Path $dir)) {
        New-Item -ItemType Directory -Force -Path $dir | Out-Null
    }

    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText(
        $Path,
        ($Text -replace "`n", "`r`n"),
        $utf8NoBom
    )
}

function Backup-Once {
    param(
        [string]$Path,
        [string]$Suffix
    )

    $backupPath = "$Path.$Suffix"

    if ((Test-Path $Path) -and -not (Test-Path $backupPath)) {
        Copy-Item $Path $backupPath
    }
}

function Find-MatchingBraceIndex {
    param(
        [string]$Text,
        [int]$OpenBraceIndex
    )

    $depth = 0

    for ($i = $OpenBraceIndex; $i -lt $Text.Length; ++$i) {
        $ch = $Text[$i]

        if ($ch -eq '{') {
            ++$depth
        } elseif ($ch -eq '}') {
            --$depth

            if ($depth -eq 0) {
                return $i
            }
        }
    }

    throw "Matching brace not found."
}

$cliPath = Join-Path $RepoRoot "tools\zero_cli.cpp"

if (-not (Test-Path $cliPath)) {
    throw "tools\zero_cli.cpp not found. Run this from D:\Zero-CPU."
}

Backup-Once -Path $cliPath -Suffix "bak_run_os_bio_os_runner"

$cli = Normalize-Newlines (Get-Content -Raw -Path $cliPath)

if (-not $cli.Contains('zero_cpu/system/BioOSRunner.hpp')) {
    $includeAnchor = '#include "zero_cpu/core/CPU.hpp"'

    if (-not $cli.Contains($includeAnchor)) {
        throw "Cannot find CPU include anchor."
    }

    $cli = $cli.Replace(
        $includeAnchor,
        $includeAnchor + "`n" + '#include "zero_cpu/system/BioOSRunner.hpp"'
    )
}

$pattern = 'int\s+runBioOSDirectory\s*\(\s*const\s+std::string&\s+\w+\s*\)\s*\{'
$match = [regex]::Match($cli, $pattern)

if (-not $match.Success) {
    throw "runBioOSDirectory function not found."
}

$openBrace = $cli.IndexOf('{', $match.Index)
$closeBrace = Find-MatchingBraceIndex -Text $cli -OpenBraceIndex $openBrace

$replacement = @'
int runBioOSDirectory(const std::string& directory) {
    using namespace zero_cpu::system;

    std::cout << "=== Zero-CPU BIO-OS Run ===\n\n";

    BioOSRunOptions options;
    options.directory = directory;

    BioOSRunner runner;
    const BioOSRunResult result = runner.run(options);

    std::cout << "Directory: "
              << result.directory
              << "\n";
    std::cout << "Generated Source: "
              << result.combined_source_path
              << "\n";
    std::cout << "Generated Binary: "
              << result.combined_binary_path
              << "\n";
    std::cout << "Instruction Count: "
              << result.instruction_count
              << "\n";
    std::cout << "Code Size: "
              << result.code_size
              << " bytes\n";
    std::cout << "Syscall Handler PC: "
              << result.syscall_handler_pc
              << "\n";
    std::cout << "Timer Handler PC: "
              << result.timer_handler_pc
              << "\n";
    std::cout << "BIO-OS Stack Base: "
              << result.stack_base
              << "\n";
    std::cout << "Step Count: "
              << result.step_count
              << "\n";
    std::cout << "Final PC: "
              << result.final_pc
              << "\n";
    std::cout << "Final SP: "
              << result.final_sp
              << "\n";
    std::cout << "Exit Code: "
              << result.exit_code
              << "\n\n";

    std::cout << "Debug Writes: "
              << result.debug_writes.size()
              << "\n";

    if (result.debug_ascii.empty()) {
        std::cout << "ASCII view: <empty>\n";
    } else {
        std::cout << "ASCII view:\n";
        std::cout << result.debug_ascii << "\n";
    }

    std::cout << "Timer tick count = "
              << result.timer_tick_count
              << "\n";
    std::cout << "Timer interval = "
              << result.timer_interval
              << "\n";
    std::cout << "Timer vector = "
              << result.timer_vector
              << "\n";
    std::cout << "Timer payload = "
              << result.timer_payload
              << "\n";
    std::cout << "Timer interrupt count = "
              << result.timer_interrupt_count
              << "\n";
    std::cout << "Timer enabled = "
              << (result.timer_enabled ? "true" : "false")
              << "\n\n";

    if (!result.success()) {
        std::cout << "BIO-OS run failed: "
                  << result.error_message
                  << "\n";
        return 1;
    }

    bool passed = true;

    auto expect = [&passed](
        const std::string& name,
        bool condition
    ) {
        std::cout << (condition ? "[PASS] " : "[FAIL] ")
                  << name
                  << "\n";

        if (!condition) {
            passed = false;
        }
    };

    expect("BIO-OS halted CPU", result.halted);
    expect("BIO-OS ASCII output is BU", result.debug_ascii == "BU");
    expect("BIO-OS debug output captured at least two writes", result.debug_writes.size() >= 2);
    expect("BIO-OS timer interrupt count is at least one", result.timer_interrupt_count >= 1);
    expect("BIO-OS timer disabled itself", !result.timer_enabled);
    expect("BIO-OS exit code is zero", result.exit_code == 0);

    if (!passed) {
        std::cout << "\nBIO-OS run failed checks.\n";
        return 1;
    }

    std::cout << "\nBIO-OS run finished successfully.\n";
    return 0;
}
'@

$cli = $cli.Substring(0, $match.Index) +
    $replacement +
    $cli.Substring($closeBrace + 1)

Save-Utf8NoBom -Path $cliPath -Text $cli

$docPath = Join-Path $RepoRoot "docs\run-os-bio-os-runner-refactor.md"
$doc = @'
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
    ↓
BioOSRunner
    ↓
BIO-OS result
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
Studio Run BIO-OS → BioOSRunner
```
'@

Save-Utf8NoBom -Path $docPath -Text $doc

$cliAfter = Normalize-Newlines (Get-Content -Raw -Path $cliPath)

if (-not $cliAfter.Contains('BioOSRunner runner;')) {
    throw "VERIFY FAILED: run-os does not contain BioOSRunner runner."
}

if (-not $cliAfter.Contains('BIO-OS run finished successfully.')) {
    throw "VERIFY FAILED: expected success message missing."
}

if (-not $cliAfter.Contains('ASCII view:')) {
    throw "VERIFY FAILED: ASCII view output missing."
}

Write-Host "PATCH OK"
Write-Host "Patched tools\zero_cli.cpp: run-os now uses BioOSRunner"
Write-Host "Created docs\run-os-bio-os-runner-refactor.md"
Write-Host ""
Write-Host "Next:"
Write-Host "  cmake --build build --target zero_cli"
Write-Host "  .\build\Debug\zero_cli.exe run-os examples\bio_os"
Write-Host "  .\build\Debug\zero_cli.exe bio-os-runner-test examples\bio_os"
