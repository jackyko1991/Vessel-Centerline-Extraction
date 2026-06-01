@echo off
setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..
set BUILD_DIR=%ROOT_DIR%\build\app
set VMTK_INSTALL=%ROOT_DIR%\build\vmtk-install

cd /d "%ROOT_DIR%"

:: Check pixi is available
where pixi >nul 2>&1
if errorlevel 1 (
    echo ERROR: pixi not found. Install from https://pixi.sh
    echo   Run in PowerShell: iwr -useb https://pixi.sh/install.ps1 ^| iex
    exit /b 1
)

echo =^> Installing pixi dependencies ^(including VMTK on Windows^)...
pixi install
if errorlevel 1 exit /b 1

:: VMTK is a conda-forge package on Windows — no source build needed.
:: build-vmtk.bat is a no-op kept for parity with the Unix scripts.
call "%SCRIPT_DIR%build-vmtk.bat"
if errorlevel 1 exit /b 1

echo =^> Configuring CenterlineExtraction...
pixi run cmake -G Ninja ^
  -S "%ROOT_DIR%" ^
  -B "%BUILD_DIR%" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DVMTK_DIR="%VMTK_INSTALL%"
if errorlevel 1 exit /b 1

echo =^> Building...
pixi run cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 exit /b 1

echo =^> Running tests...
pixi run ctest --test-dir "%BUILD_DIR%" --output-on-failure
if errorlevel 1 exit /b 1

echo.
echo Build complete. Binary: %BUILD_DIR%\CenterlineExtraction.exe
