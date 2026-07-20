@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0\.."

echo.
echo ========================================
echo Zero-CPU Test Suite
echo ========================================
echo.

echo [1/14] Building project...
cmake --build build
if errorlevel 1 goto fail

set "ZERO_CLI=build\Debug\zero_cli.exe"

if not exist "%ZERO_CLI%" (
    set "ZERO_CLI=build\Release\zero_cli.exe"
)

if not exist "%ZERO_CLI%" (
    echo.
    echo ERROR: zero_cli.exe not found.
    echo Tried:
    echo   build\Debug\zero_cli.exe
    echo   build\Release\zero_cli.exe
    goto fail
)

echo.
echo Using CLI:
echo   %ZERO_CLI%
echo.

echo [2/14] Running ALU unit test...
"%ZERO_CLI%" alu-test
if errorlevel 1 goto fail

echo.
echo [3/14] Running MMIO bus test...
"%ZERO_CLI%" mmio-test
if errorlevel 1 goto fail

echo.
echo [4/14] Running interrupt controller test...
"%ZERO_CLI%" interrupt-test
if errorlevel 1 goto fail

echo.
echo [5/14] Running CPU interrupt delivery test...
"%ZERO_CLI%" cpu-interrupt-test
if errorlevel 1 goto fail

echo.
echo [6/14] Running timer device test...
"%ZERO_CLI%" timer-test
if errorlevel 1 goto fail

echo.
echo [7/14] Running CPU timer interrupt test...
"%ZERO_CLI%" cpu-timer-test
if errorlevel 1 goto fail

echo.
echo [8/14] Running CPU EI/DI interrupt control test...
"%ZERO_CLI%" cpu-ei-di-test
if errorlevel 1 goto fail


echo.
echo [9/14] Running software interrupt test...
"%ZERO_CLI%" software-interrupt-test
if errorlevel 1 goto fail

echo.
echo [11/14] Running binary format round-trip test...
"%ZERO_CLI%" binary-test
if errorlevel 1 goto fail

echo.
echo [12/14] Assembling and running function_call.zasm...
"%ZERO_CLI%" assemble "examples\function_call.zasm" "examples\function_call.zbin"
if errorlevel 1 goto fail

"%ZERO_CLI%" run-binary "examples\function_call.zbin" --expect-memory 100=20 2048=20
if errorlevel 1 goto fail

echo.
echo [13/14] Assembling and running alu_flags.zasm...
"%ZERO_CLI%" assemble "examples\alu_flags.zasm" "examples\alu_flags.zbin"
if errorlevel 1 goto fail

"%ZERO_CLI%" run-binary "examples\alu_flags.zbin" --expect-memory 120=30 128=20 136=1 144=2 152=3 160=4 168=5 200=777
if errorlevel 1 goto fail

echo.
echo [14/14] Assembling and running mmio_output.zasm...
"%ZERO_CLI%" assemble "examples\mmio_output.zasm" "examples\mmio_output.zbin"
if errorlevel 1 goto fail

"%ZERO_CLI%" run-binary "examples\mmio_output.zbin" --debug-mmio --expect-memory 220=66 228=2
if errorlevel 1 goto fail

echo.
echo ========================================
echo All Zero-CPU tests passed.
echo ========================================
echo.

exit /b 0

:fail
echo.
echo ========================================
echo Zero-CPU tests failed.
echo ========================================
echo.

exit /b 1