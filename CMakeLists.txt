cmake_minimum_required(VERSION 3.16)
project(CenterlineExtraction VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VTK conda-forge build requires Python3 to be found first (it was built with Python wrapping)
find_package(Python3 COMPONENTS Interpreter Development)

# VTK 9 — no VTK_USE_FILE, use vtk_module_autoinit instead
find_package(VTK 9.0 REQUIRED COMPONENTS
  CommonCore
  CommonDataModel
  FiltersCore
  FiltersGeneral
  FiltersSources
  IOGeometry
  IOXML
  RenderingCore
  RenderingOpenGL2
  RenderingAnnotation
  RenderingVolume
  InteractionStyle
  RenderingFreeType)

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# VMTK — skip VMTKUse.cmake: it calls link_libraries() globally and includes
# vtkvmtkContrib which is not built. Instead, set include dirs manually and
# link only the installed libraries.
#
# Platform layout:
#   macOS   — source-built VMTK; VMTK_DIR = build/vmtk-install/lib,
#              VMTKConfig.cmake lives directly in that dir.
#   Windows — conda-forge; VMTK_DIR = $CONDA_PREFIX/Library,
#              no VMTKConfig.cmake, libs in VMTK_DIR/lib/.
#   Linux   — conda-forge; VMTK_DIR = $CONDA_PREFIX,
#              no VMTKConfig.cmake, libs in VMTK_DIR/lib/.
set(VMTK_DIR "" CACHE PATH "Path to VMTK install/conda-prefix directory")
if(APPLE)
  find_package(VMTK REQUIRED HINTS ${VMTK_DIR} $ENV{VMTK_DIR})
  set(VMTK_LIB_DIR "${VMTK_DIR}")
elseif(WIN32)
  set(VMTK_INCLUDE_DIRS "${VMTK_DIR}/include/vmtk")
  set(VMTK_LIB_DIR "${VMTK_DIR}/lib")
  message(STATUS "Windows: VMTK headers at ${VMTK_INCLUDE_DIRS}")
else()
  set(VMTK_INCLUDE_DIRS "${VMTK_DIR}/include/vmtk")
  set(VMTK_LIB_DIR "${VMTK_DIR}/lib")
  message(STATUS "Linux: VMTK headers at ${VMTK_INCLUDE_DIRS}")
endif()

# Sources — explicit list, no GLOB
set(SOURCES
  src/main.cpp
  src/Centerline.cpp
  src/interactorStyleCenterline.cpp
  src/NonManifoldSurfaceChecker.cpp)

add_executable(CenterlineExtraction MACOSX_BUNDLE ${SOURCES})

set_target_properties(CenterlineExtraction PROPERTIES
  # macOS bundle metadata — sets the Dock/menu bar app name
  MACOSX_BUNDLE_BUNDLE_NAME        "Vessel Centerline Extraction"
  MACOSX_BUNDLE_GUI_IDENTIFIER     "org.jackyko1991.vessel-centerline-extraction"
  MACOSX_BUNDLE_BUNDLE_VERSION     "${PROJECT_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
  # Embed rpaths so the binary finds VMTK libs without DYLD_LIBRARY_PATH / LD_LIBRARY_PATH
  BUILD_WITH_INSTALL_RPATH TRUE
  INSTALL_RPATH "${VMTK_LIB_DIR}")

# Collect installed VMTK libraries (avoids vtkvmtkContrib which isn't built)
if(WIN32)
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/vtkvmtk*.lib")
elseif(APPLE)
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/libvtkvmtk*.dylib")
else()
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/libvtkvmtk*.so")
endif()

target_include_directories(CenterlineExtraction PRIVATE
  include
  ${VMTK_INCLUDE_DIRS}
  ${ITK_INCLUDE_DIRS})

target_link_directories(CenterlineExtraction PRIVATE "${VMTK_LIB_DIR}")

target_link_libraries(CenterlineExtraction PRIVATE
  ${VTK_LIBRARIES}
  ${ITK_LIBRARIES}
  ${VMTK_INSTALLED_LIBS})

# macOS only: VMTK dylibs use flat-namespace lookup for libpython symbols.
# Link libpython explicitly so dyld resolves it at load time.
if(APPLE)
  find_library(PYTHON_LIB
    NAMES python3.14 python3.13 python3.12 python3.11 python3
    HINTS "${Python3_ROOT_DIR}/lib" "$ENV{CONDA_PREFIX}/lib"
    NO_DEFAULT_PATH)
  if(PYTHON_LIB)
    message(STATUS "Linking libpython for flat-namespace resolution: ${PYTHON_LIB}")
    target_link_options(CenterlineExtraction PRIVATE
      "LINKER:-needed_library,${PYTHON_LIB}")
  else()
    message(WARNING "libpython not found — runtime may fail if VMTK uses Python wrapping")
  endif()
endif()

# Required for VTK 9 factory initialisation
vtk_module_autoinit(
  TARGETS CenterlineExtraction
  MODULES ${VTK_LIBRARIES})

# Testing
include(CTest)
enable_testing()
add_subdirectory(tests)

# Install
install(TARGETS CenterlineExtraction
  BUNDLE  DESTINATION .
  RUNTIME DESTINATION bin)
