#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"
VMTK_SRC="$BUILD_DIR/vmtk-src"
VMTK_BUILD="$BUILD_DIR/vmtk-build"
VMTK_INSTALL="$BUILD_DIR/vmtk-install"

if [ -d "$VMTK_INSTALL/lib" ]; then
  echo "VMTK already built at $VMTK_INSTALL, skipping."
  echo "Delete $VMTK_INSTALL to force rebuild."
  exit 0
fi

if [ -z "${CONDA_PREFIX:-}" ]; then
  echo "ERROR: CONDA_PREFIX not set. Run inside pixi environment: pixi run bash scripts/build-vmtk.sh"
  exit 1
fi

echo "==> Cloning VMTK..."
mkdir -p "$BUILD_DIR"
if [ ! -d "$VMTK_SRC" ]; then
  git clone https://github.com/vmtk/vmtk.git "$VMTK_SRC"
fi

NPROC="$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"

# Export LIBRARY_PATH so the macOS linker finds bare -lxxx references
# (e.g. -lfftw3_threads) from ITK's cmake interface flags in the conda prefix.
export LIBRARY_PATH="${CONDA_PREFIX}/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"

echo "==> Configuring VMTK..."
mkdir -p "$VMTK_BUILD"

# Locate VTK and ITK cmake config dirs inside the pixi prefix
VTK_CMAKE_DIR="$(find "$CONDA_PREFIX/lib/cmake" -maxdepth 1 -name "vtk*" -type d 2>/dev/null | head -1)"
ITK_CMAKE_DIR="$(find "$CONDA_PREFIX/lib/cmake" -maxdepth 1 -name "ITK*" -type d 2>/dev/null | head -1)"

if [ -z "$VTK_CMAKE_DIR" ]; then
  echo "ERROR: VTK cmake config not found under $CONDA_PREFIX/lib/cmake"
  exit 1
fi
if [ -z "$ITK_CMAKE_DIR" ]; then
  echo "ERROR: ITK cmake config not found under $CONDA_PREFIX/lib/cmake"
  exit 1
fi

echo "  VTK_DIR: $VTK_CMAKE_DIR"
echo "  ITK_DIR: $ITK_CMAKE_DIR"

# Write a cmake toolchain file so the conda lib dir is searched first.
# ITK's cmake targets export bare -lfftw3_threads flags; on macOS ld,
# -L must appear before -l references in the link line. The toolchain
# sets CMAKE_*_LINKER_FLAGS early so cmake propagates the -L into every
# generated ninja rule before target link libraries are appended.
mkdir -p "$VMTK_BUILD"
TOOLCHAIN="$VMTK_BUILD/conda_toolchain.cmake"
cat > "$TOOLCHAIN" << TOOLEOF
set(CMAKE_SHARED_LINKER_FLAGS "-L$CONDA_PREFIX/lib -Wl,-rpath,$CONDA_PREFIX/lib \${CMAKE_SHARED_LINKER_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS    "-L$CONDA_PREFIX/lib -Wl,-rpath,$CONDA_PREFIX/lib \${CMAKE_EXE_LINKER_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS "-L$CONDA_PREFIX/lib \${CMAKE_MODULE_LINKER_FLAGS}" CACHE STRING "" FORCE)
TOOLEOF

cmake -G Ninja \
  -S "$VMTK_SRC" \
  -B "$VMTK_BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$VMTK_INSTALL" \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
  -DVMTK_USE_VTK_FROM_SUPERBUILD=OFF \
  -DVMTK_USE_ITK_FROM_SUPERBUILD=OFF \
  -DVMTK_USE_SUPERBUILD=OFF \
  -DVTK_DIR="$VTK_CMAKE_DIR" \
  -DITK_DIR="$ITK_CMAKE_DIR" \
  -DBUILD_SHARED_LIBS=ON \
  -DVMTK_BUILD_TESTING=OFF \
  -DVMTK_BUILD_DOCUMENTATION=OFF \
  -DVMTK_WITH_RENDERING=ON \
  -DVMTK_WRAP_PYTHON=OFF

echo "==> Building VMTK (this takes ~20-40 minutes on first run)..."
cmake --build "$VMTK_BUILD" --parallel "$NPROC"
cmake --install "$VMTK_BUILD"
echo "==> VMTK installed to $VMTK_INSTALL"
