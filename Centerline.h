#ifndef CENTERLINE_H
#define CENTERLINE_H

#include <vtkPolyData.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkScalarBarActor.h>

#include <vtkvmtkCapPolyData.h>
#include <vtkvmtkPolyDataCenterlines.h>

using namespace std;

class Centerline
{
public:

	Centerline();
	~Centerline();

	void SetSurface(vtkPolyData* surface); // this function is for uncapped surface, this can automatically detect source and target seeds, may modify via SetSourceIds/SetTargetIds function
	void SetCappedSurface(vtkPolyData* cappedSurface); // this function is for capped/enclosed surface, need manually set source and target seeds
	void SetSourceIds(vtkIdList* sourceIds);
	void SetTargetIds(vtkIdList* targetIds);
	void SetLookupTable(vtkLookupTable* LUT);
	void SetAppendEndPoints(bool appendFlag);
	void Update();
	
	void GetSurface(vtkPolyData* surface);
	void GetCappedSurface(vtkPolyData* cappedSurface);
	void GetCenterline(vtkPolyData* centerline);
	vtkPolyData* GetCenterline();
	void GetLookupTable(vtkLookupTable* LUT);

private:
	vtkSmartPointer<vtkPolyData>			m_surface;
	vtkSmartPointer<vtkPolyData>			m_centerline;
	vtkSmartPointer<vtkPolyData>			m_cappedSurface;
	vtkSmartPointer<vtkLookupTable>			m_LUT;
	vtkSmartPointer<vtkIdList>				m_sourceIds;
	vtkSmartPointer<vtkIdList>				m_targetIds;

	bool m_hasCappedSurface = 0;
	bool m_appendEndPointsFlag = 0;

	void CalculateCenterline();
	void CapSurface(vtkPolyData* inputSurface, vtkPolyData* cappedSurface, vtkIdList* CapCenterIds);
	void CreateLookupTable();
};

#endif
