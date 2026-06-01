#include "NonManifoldSurfaceChecker.h"
#include <iostream>

NonManifoldSurfaceChecker::NonManifoldSurfaceChecker()
{
	m_surface = NULL;
}

NonManifoldSurfaceChecker::~NonManifoldSurfaceChecker()
{
}

void NonManifoldSurfaceChecker::SetSurface(vtkPolyData* surface)
{
	m_surface = surface;
}

void NonManifoldSurfaceChecker::Update()
{
	if (m_surface == NULL)
		std::cerr << "NonManifoldSurfaceChecker error: Surface not set";
}
