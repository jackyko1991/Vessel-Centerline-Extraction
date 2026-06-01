@echo off
setlocal enabledelayedexpansion

:: On Windows, VMTK is available as a prebuilt conda-forge package.
:: This script is a no-op — pixi install handles it via pixi.toml.
:: It exists so build.bat can call it unconditionally on all platforms.

echo VMTK is managed by pixi on Windows (conda-forge package).
echo Nothing to build from source.
exit /b 0
