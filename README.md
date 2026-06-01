# Vessel Centerline Extraction

An interactive CLI tool to extract vessel centerlines from surface meshes. Open vessel ends are automatically capped during processing.

<p align="left">
<img src="./doc/img/result_large.jpg" width="359px" height="266px" title="Result"/>
</p>

[![CI](https://github.com/jackyko1991/Vessel-Centerline-Extraction/actions/workflows/build.yml/badge.svg)](https://github.com/jackyko1991/Vessel-Centerline-Extraction/actions/workflows/build.yml)

## Output Arrays

The output centerline `.vtp` file contains:

| Array | Description |
|-------|-------------|
| Abscissas | Arc-length along centerline |
| Curvature | Local curvature |
| FrenetBinormal / FrenetNormal / FrenetTangent | Frenet frame vectors |
| ParallelTransportNormals | Parallel transport frame |
| Radius | Inscribed sphere radius (vessel lumen radius) |
| Torsion | Local torsion |

## Usage

After building, run the binary directly (do **not** use `open` on macOS — the bundle will close immediately):

```bash
# macOS
./build/app/CenterlineExtraction.app/Contents/MacOS/CenterlineExtraction \
  <surface.stl> <capped_surface.stl> <centerline.vtp>

# Linux
./build/app/CenterlineExtraction <surface.stl> <capped_surface.stl> <centerline.vtp>
```

Example with the bundled test data:

```bash
# macOS
./build/app/CenterlineExtraction.app/Contents/MacOS/CenterlineExtraction \
  test_data/left/lumen.stl test_data/left/leftlumen_capped.stl /tmp/out.vtp

# Linux
./build/app/CenterlineExtraction \
  test_data/left/lumen.stl test_data/left/leftlumen_capped.stl /tmp/out.vtp
```

Output is in VTK PolyData format (`.vtp`). Open with [ParaView](https://www.paraview.org/).

## Interaction Keys

| Key | Action |
|-----|--------|
| `n` | Add a new seed point at the origin |
| `Space` | Place the current seed at the cursor position |
| `Tab` | Toggle seed type — **red** = inlet, **green** = outlet |
| `Enter` | Compute centerline from placed seeds |
| `Q` | Quit |

## Build from Source

### Prerequisites

Install [pixi](https://pixi.sh):

```bash
curl -fsSL https://pixi.sh/install.sh | bash
```

### Build

```bash
git clone https://github.com/jackyko1991/Vessel-Centerline-Extraction
cd Vessel-Centerline-Extraction
bash scripts/build.sh
```

The script will:
1. Install VTK 9, ITK, CMake and Ninja via pixi (conda-forge) — no system installs needed
2. Build VMTK from source into `build/vmtk-install/` (~20–40 min on first run, skipped if cached)
3. Configure and compile `CenterlineExtraction`
4. Run unit tests against the bundled test data

### Platforms

| Platform | Status |
|----------|--------|
| macOS Apple Silicon (arm64) | Supported |
| macOS Intel (x86_64) | Supported |
| Linux (x86_64) | Supported (CI tested) |
| Windows (x86_64) | Supported |

On Windows, run `scripts\build.bat` from a Command Prompt instead. VMTK is installed automatically as a prebuilt conda-forge package — no source build required.

## GUI Version

A Qt-based GUI version is available at [Vessel-Clipper](https://github.com/jackyko1991/Vessel-Clipper).
