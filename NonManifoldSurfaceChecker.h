#ifndef NONMANIFOLDSURFACECHECKER_H
#define NONMANIFOLDSURFACECHECKER_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkvmtkNeighborhood.h>

using namespace std;

class NonManifoldSurfaceChecker
{
public:
	NonManifoldSurfaceChecker();
	~NonManifoldSurfaceChecker();

	void SetSurface(vtkPolyData* surface);
	void Update();

private:
	vtkPolyData* m_surface;
	bool NonMaifoldEdgesFound = 0;
};

#endif