# Build Instructions

## Quick Start

Install [pixi](https://pixi.sh):

- **macOS / Linux:** `curl -fsSL https://pixi.sh/install.sh | bash`
- **Windows (PowerShell):** `iwr -useb https://pixi.sh/install.ps1 | iex`

Then clone and build:

**macOS / Linux:**
```bash
git clone https://github.com/jackyko1991/Vessel-Centerline-Extraction
cd Vessel-Centerline-Extraction
bash scripts/build.sh
```

**Windows (Command Prompt):**
```bat
git clone https://github.com/jackyko1991/Vessel-Centerline-Extraction
cd Vessel-Centerline-Extraction
scripts\build.bat
```

That's it. The script handles everything automatically.

## What the Build Script Does

| Step | macOS arm64 | Linux | Windows |
|------|-------------|-------|---------|
| 1. Dependencies | `pixi install` — VTK 9, ITK, CMake, Ninja via conda-forge | Same, plus VMTK prebuilt package | Same, plus VMTK prebuilt package |
| 2. VMTK | Built from source (~20–40 min, cached after first run) | Installed by pixi (prebuilt conda-forge package) | Installed by pixi (prebuilt conda-forge package) |
| 3. Configure | `cmake … -DVMTK_DIR=build/vmtk-install/lib` | `cmake … -DVMTK_DIR=$CONDA_PREFIX` | `cmake …` (VMTK found via conda prefix) |
| 4. Build | `cmake --build` via ninja | Same | Same |
| 5. Test | `ctest` | Same | Same |

> **Why macOS builds VMTK from source:** no `arm64` conda-forge package exists for VMTK. Linux and Windows use the prebuilt package.

## Manual Build (for Developers)

**macOS arm64:**
```bash
# Enter pixi environment
pixi shell

# Build VMTK from source (first time only, ~20-40 min)
bash scripts/build-vmtk.sh

# Configure
cmake -G Ninja \
  -S . \
  -B build/app \
  -DCMAKE_BUILD_TYPE=Release \
  -DVMTK_DIR=build/vmtk-install/lib

# Build
cmake --build build/app --parallel

# Test
ctest --test-dir build/app --output-on-failure

# Run
./build/app/CenterlineExtraction.app/Contents/MacOS/CenterlineExtraction \
  test_data/left/lumen.stl test_data/left/leftlumen_capped.stl out.vtp
```

**Linux:**
```bash
# Enter pixi environment (installs VMTK via conda-forge automatically)
pixi shell

# Configure — VMTK is in the conda prefix
cmake -G Ninja \
  -S . \
  -B build/app \
  -DCMAKE_BUILD_TYPE=Release \
  -DVMTK_DIR=$CONDA_PREFIX

# Build
cmake --build build/app --parallel

# Test
ctest --test-dir build/app --output-on-failure

# Run
./build/app/CenterlineExtraction test_data/left/lumen.stl test_data/left/leftlumen_capped.stl out.vtp
```

**Windows (Command Prompt):**
```bat
:: Enter pixi environment
pixi shell

:: VMTK is already installed by pixi — no source build needed

:: Configure (VMTK is on PATH via conda prefix)
cmake -G Ninja -S . -B build\app -DCMAKE_BUILD_TYPE=Release

:: Build
cmake --build build\app --parallel

:: Test
ctest --test-dir build\app --output-on-failure

:: Run
build\app\CenterlineExtraction.exe test_data\left\lumen.stl test_data\left\leftlumen_capped.stl out.vtp
```

## Troubleshooting

**Force a VMTK rebuild (macOS only):**
```bash
rm -rf build/vmtk-install
bash scripts/build.sh
```

**VTK or ITK not found during configure:**
Make sure cmake runs inside the pixi environment. Use `pixi run cmake ...` or `pixi shell` first.

**`CONDA_PREFIX` not set:**
The VMTK build script must run inside the pixi environment. Use `pixi run bash scripts/build-vmtk.sh` rather than calling it directly.

**Build fails on Apple Silicon:**
Ensure Rosetta is not active for your terminal. Run `arch` — it should print `arm64`.

**Windows: `cmake` or `ninja` not found:**
Run from inside the pixi environment (`pixi shell` or prefix commands with `pixi run`).

**Windows: VMTK headers not found during configure:**
VMTK is installed into the pixi conda prefix. Make sure cmake is run via `pixi run cmake ...` so `CONDA_PREFIX` is set and cmake can locate the package.
