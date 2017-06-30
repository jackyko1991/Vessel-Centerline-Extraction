#include "NonManifoldSurfaceChecker.h"

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
		cerr << "NonManifoldSurfaceChecker error: Surface not set";
	//else
	//{
	//	vtkSmartPointer<vtkvmtkNeighborhood> neighborhoods = vtkSmartPointer<vtkvmtkNeighborhood>::New();
	//	neighborhoods->
	//		SetNeighborhoodTypeToPolyDataManifoldNeighborhood();
	//	neighborhoods->SetDataSet(m_surface);
	//	neighborhoods->Build();
	//}

	//	self.NonManifoldEdgesFound = 0
	//	self.Report = ''
	//	self.NonManifoldEdgePointIds.Initialize()

	//	neighborhoods = vtkvmtk.vtkvmtkNeighborhoods()
	//	neighborhoods.SetNeighborhoodTypeToPolyDataManifoldNeighborhood()
	//	neighborhoods.SetDataSet(self.Surface)
	//	neighborhoods.Build()

	//	neighborCellIds = vtk.vtkIdList()
	//	cellPointIds = vtk.vtkIdList()

	//	self.Surface.BuildCells()
	//	self.Surface.BuildLinks(0)

	//	numberOfNonManifoldEdges = 0

	//	for i in range(neighborhoods.GetNumberOfNeighborhoods()) :

	//		neighborhood = neighborhoods.GetNeighborhood(i)

	//		for j in range(neighborhood.GetNumberOfPoints()) :

	//			neighborId = neighborhood.GetPointId(j)

	//			if (i<neighborId) :

	//				neighborCellIds.Initialize()
	//				self.Surface.GetCellEdgeNeighbors(-1, i, neighborId, neighborCellIds)

	//				if (neighborCellIds.GetNumberOfIds()>2) :

	//					numberOfNonManifoldEdges = numberOfNonManifoldEdges + 1

	//					self.Report = self.Report + "Non-manifold edge found" + str(i) + ' ' + str(neighborId) + '.\n'

	//					self.NonManifoldEdgePointIds.InsertNextId(i)
						//self.NonManifoldEdgePointIds.InsertNextId(neighborId)
}