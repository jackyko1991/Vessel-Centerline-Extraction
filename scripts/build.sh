#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build/app"
VMTK_INSTALL="$ROOT_DIR/build/vmtk-install"

cd "$ROOT_DIR"

if ! command -v pixi &>/dev/null; then
  echo "ERROR: pixi not found. Install from https://pixi.sh"
  echo "  curl -fsSL https://pixi.sh/install.sh | bash"
  exit 1
fi

echo "==> Installing pixi dependencies..."
pixi install

OS="$(uname -s)"
if [[ "$OS" == "Darwin" ]]; then
  echo "==> Building VMTK from source (no conda-forge package for macOS arm64)..."
  pixi run bash scripts/build-vmtk.sh
  VMTK_DIR="$VMTK_INSTALL/lib"
else
  echo "==> Using conda-forge VMTK (Linux)..."
  VMTK_DIR="$(pixi run bash -c 'echo $CONDA_PREFIX')"
fi

# LIBRARY_PATH lets the linker find bare -lxxx flags from ITK's cmake targets
# (e.g. -lfftw3_threads) in the pixi prefix.
# macOS: VMTK_DIR is already the lib dir; Linux: libs are in VMTK_DIR/lib.
if [[ "$OS" == "Darwin" ]]; then
  export LIBRARY_PATH="$VMTK_DIR:${LIBRARY_PATH:-}"
else
  export LIBRARY_PATH="$VMTK_DIR/lib:${LIBRARY_PATH:-}"
fi

echo "==> Configuring CenterlineExtraction..."
pixi run cmake -G Ninja \
  -S "$ROOT_DIR" \
  -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DVMTK_DIR="$VMTK_DIR"

echo "==> Building..."
NPROC="$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"
pixi run cmake --build "$BUILD_DIR" --parallel "$NPROC"

echo "==> Running tests..."
pixi run ctest --test-dir "$BUILD_DIR" --output-on-failure

echo ""
if [ -d "$BUILD_DIR/CenterlineExtraction.app" ]; then
  BIN="$BUILD_DIR/CenterlineExtraction.app/Contents/MacOS/CenterlineExtraction"
else
  BIN="$BUILD_DIR/CenterlineExtraction"
fi
echo ""
echo "Build complete."
echo "  Run: $BIN <surface.stl> <capped_surface.stl> <centerline.vtp>"
