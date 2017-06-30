# Vessel-Centerline-Extraction
An interactive software to extract vessel centerline. The open end vessel will be enclosed in the process. Following parameters will be included in the output centerline vtp file:

- Abscissas
- Curvature
- Fenet system (Binomal, Normal, Tangent)
- Parallel Transport Normals
- Radius
- Torsion

Required packages: CMake VTK, VMTK

# Usage
Centerline.exe <surface.stl> <capped_surface.stl> <centerline.vtp>

# Interaction keys:
n - New inlet/outlet point
space - locating the inlet/outlet point
tab - change inlet/outlet point type (red: inlet, green: outlet)
enter - process
