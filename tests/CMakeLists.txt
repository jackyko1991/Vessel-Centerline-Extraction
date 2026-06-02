cmake_minimum_required(VERSION 3.16)

find_package(Python3 COMPONENTS Interpreter Development)

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
  InteractionStyle
  RenderingFreeType
  RenderingAnnotation
  RenderingVolume)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if(APPLE)
  find_package(VMTK REQUIRED HINTS ${VMTK_DIR} $ENV{VMTK_DIR})
  set(VMTK_LIB_DIR "${VMTK_DIR}")
elseif(WIN32)
  set(VMTK_INCLUDE_DIRS "${VMTK_DIR}/include/vmtk")
  set(VMTK_LIB_DIR "${VMTK_DIR}/lib")
else()
  set(VMTK_INCLUDE_DIRS "${VMTK_DIR}/include/vmtk")
  set(VMTK_LIB_DIR "${VMTK_DIR}/lib")
endif()

get_filename_component(_py_lib_dir "${Python3_LIBRARY}" DIRECTORY)

add_executable(test_centerline
  test_centerline.cpp
  ${CMAKE_SOURCE_DIR}/src/Centerline.cpp)

# Embed rpaths so dyld finds pixi libs (Python, VMTK) without DYLD_LIBRARY_PATH.
# macOS SIP strips DYLD_LIBRARY_PATH from child processes, so set_tests_properties
# ENVIRONMENT won't work — rpaths in the binary are the only reliable solution.
set_target_properties(test_centerline PROPERTIES
  BUILD_WITH_INSTALL_RPATH TRUE
  INSTALL_RPATH "${VMTK_LIB_DIR};${_py_lib_dir}")

target_include_directories(test_centerline PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${VMTK_INCLUDE_DIRS}
  ${ITK_INCLUDE_DIRS})

target_compile_features(test_centerline PRIVATE cxx_std_17)

if(WIN32)
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/vtkvmtk*.lib")
elseif(APPLE)
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/libvtkvmtk*.dylib")
else()
  file(GLOB VMTK_INSTALLED_LIBS "${VMTK_LIB_DIR}/libvtkvmtk*.so")
endif()

target_link_directories(test_centerline PRIVATE "${VMTK_LIB_DIR}")

target_link_libraries(test_centerline PRIVATE
  ${VTK_LIBRARIES}
  ${ITK_LIBRARIES}
  ${VMTK_INSTALLED_LIBS})

if(APPLE AND PYTHON_LIB)
  target_link_options(test_centerline PRIVATE
    "LINKER:-needed_library,${PYTHON_LIB}")
endif()

vtk_module_autoinit(TARGETS test_centerline MODULES ${VTK_LIBRARIES})

target_compile_definitions(test_centerline PRIVATE
  TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/test_data")

add_test(NAME CenterlineLeft COMMAND test_centerline left)
add_test(NAME CenterlineRight COMMAND test_centerline right)

set_tests_properties(CenterlineLeft CenterlineRight PROPERTIES TIMEOUT 120)
