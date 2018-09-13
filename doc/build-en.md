This compilation is tested with MSVC 2015. It is recommended to build toolchains from sources for better version compatibility. 

# Prerequisites
Please install the following packages.
- [Git for Windows](https://git-scm.com/)
- MSVC 2015 (Windows)
- G++ (Ubuntu `sudo apt install build-essential`)
- [CMake](https://cmake.org/)
- [VTK](https://github.com/Kitware/VTK.git) (Mandatory when VMTK super build is not implemented)
- [ITK](https://github.com/InsightSoftwareConsortium/ITK.git) (Mandatory when VMTK super build is not implemented)
- [Qt](https://www.qt.io/)
- [VMTK](https://github.com/vmtk/vmtk)

Please following the build instructions from each package. If you have any difficulty in building the library, please don't hesitate to contact me.

# Compilation steps
1. Create an empty folder anywhere in the computer, e.g. `D:/projects`
2. Open Git Bash and move working directory to `D:/project`
3. Type `git clone https://github.com/jackyko1991/Vessel-Centerline-Extraction` on Git Bash
4. Create an empty folder `D:/projects/Vessel-Centerline-Extraction/build`
5. Open CMake GUI
2. Press `Browse Source` and direct to `D:/projects/Vessel-Centerline-Extraction`
3. Press `Browse Build` and direct to `D:/projects/Vessel-Centerline-Extraction/build`
4. Press `Configure` and choose `Visual Studio 14 2015 Win64` (Windows) or `Unix Makefiles` (Ubuntu)
5. Error comes out when configuring VMTK, change the `VMTK_DIR` variable to the folder containing `FindVMTK.cmake`, usaually at `D:/VMTK/build/VMTK-Build`. The system should auto configure VTK paths.
6. If VTK is not found , provide the variable `VTK_DIR` to the folder containing `FindVMTK.cmake`
7. Press `Generate`
8. Build steps differ from Windows and Linux at this point
	- Windows Build
		1. Press `Open` and change solution build type to `Release`
		2. Press `F7` to compile
		3. Copy the necessary dlls to `D:\projects\Vessel-Centerline-Extraction\build\Release` upon system request. The dlls may found in VTK, VMTK and Qt binary directory
	- Ubuntu Build
		1. On terminal, move working directory to `/<projects>/Vessel-Centerline-Extraction/build`, where `/<projects>` is the directory containing the source
		2. `make`
9. Run the test script `./CenterlineExtraction <surface.stl> <capped_surface.stl> <centerline.vtp>`, replace lines in `<>` to the actual path of containing the associated files. Sample data can be found from `Vessel-Centerline-Extraction/test_data`