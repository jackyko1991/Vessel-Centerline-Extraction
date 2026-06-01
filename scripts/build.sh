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

echo "==> Building VMTK from source..."
pixi run bash scripts/build-vmtk.sh

# LIBRARY_PATH lets the linker find bare -lxxx flags from ITK's cmake targets
# (e.g. -lfftw3_threads) in the pixi prefix, and ensures the VMTK install lib
# dir is searched when linking the main project.
export LIBRARY_PATH="$VMTK_INSTALL/lib:$(pixi run python3 -c 'import sys; print(__import__("sysconfig").get_path("stdlib"))' 2>/dev/null | xargs dirname 2>/dev/null):${LIBRARY_PATH:-}"

echo "==> Configuring CenterlineExtraction..."
pixi run cmake -G Ninja \
  -S "$ROOT_DIR" \
  -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DVMTK_DIR="$VMTK_INSTALL/lib"

echo "==> Building..."
NPROC="$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"
pixi run cmake --build "$BUILD_DIR" --parallel "$NPROC"

echo "==> Running tests..."
pixi run ctest --test-dir "$BUILD_DIR" --output-on-failure

echo ""
if [ -d "$BUILD_DIR/CenterlineExtraction.app" ]; then
  echo "Build complete."
  echo "  macOS app: $BUILD_DIR/CenterlineExtraction.app"
  echo "  Run:       open $BUILD_DIR/CenterlineExtraction.app --args <surface.stl> <capped.stl> <out.vtp>"
else
  echo "Build complete. Binary: $BUILD_DIR/CenterlineExtraction"
fi
