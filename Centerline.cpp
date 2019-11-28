#include "Centerline.h"

Centerline::Centerline()
{
	m_surface = NULL;
	m_cappedSurface = NULL;
	m_centerline = vtkSmartPointer<vtkPolyData>::New();
	m_LUT = vtkSmartPointer<vtkLookupTable>::New();
}

Centerline::~Centerline()
{

}

void Centerline::SetSurface(vtkPolyData* surface)
{
	m_surface = surface;
	
	// obtain cap centers automatically
	vtkSmartPointer<vtkIdList> CapCenterIds = vtkSmartPointer<vtkIdList>::New();
	this->CapSurface(surface, m_cappedSurface, CapCenterIds);

	vtkSmartPointer<vtkIdList> sourceIds = vtkSmartPointer<vtkIdList>::New();
	sourceIds->SetNumberOfIds(1);
	sourceIds->SetId(0, CapCenterIds->GetId(0));
	
	vtkSmartPointer<vtkIdList> targetIds = vtkSmartPointer<vtkIdList>::New();
	targetIds->SetNumberOfIds(CapCenterIds->GetNumberOfIds() - 1);
	for (int i = 1; i < CapCenterIds->GetNumberOfIds(); i++)
	{
		targetIds->SetId(i - 1, CapCenterIds->GetId(i));
	}

	m_sourceIds = sourceIds;
	m_targetIds = targetIds;

	m_hasCappedSurface = 1;
}

void Centerline::SetCappedSurface(vtkPolyData* cappedSurface)
{
	m_cappedSurface = cappedSurface;
	m_hasCappedSurface = 1;
}

void Centerline::SetAppendEndPoints(bool appendFlag)
{
	m_appendEndPointsFlag = appendFlag;
}

void Centerline::SetSourceIds(vtkIdList* sourcdIds)
{
	m_sourceIds = sourcdIds;
}

void Centerline::SetTargetIds(vtkIdList* targetIds)
{
	m_targetIds = targetIds;
}

void Centerline::CapSurface(vtkPolyData* inputSurface, vtkPolyData* cappedSurface, vtkIdList* CapCenterIds)
{
	m_cappedSurface = vtkSmartPointer<vtkPolyData>::New();

	// calculate centerlines of lumen and vessl outer wall
	vtkSmartPointer<vtkvmtkCapPolyData> capper = vtkSmartPointer<vtkvmtkCapPolyData>::New();
	capper->SetInputData(inputSurface);
	capper->Update();
	cappedSurface->DeepCopy(capper->GetOutput());
	CapCenterIds->DeepCopy(capper->GetCapCenterIds());
}

void Centerline::Update()
{
	this->CalculateCenterline();
}

void Centerline::CalculateCenterline()
{
	vtkSmartPointer<vtkvmtkPolyDataCenterlines> centerlinesFilter = vtkSmartPointer<vtkvmtkPolyDataCenterlines>::New();
	centerlinesFilter->SetInputData(m_cappedSurface);
	centerlinesFilter->SetSourceSeedIds(m_sourceIds);
	centerlinesFilter->SetTargetSeedIds(m_targetIds);
	centerlinesFilter->SetAppendEndPointsToCenterlines(m_appendEndPointsFlag);
	centerlinesFilter->SetCenterlineResampling(true);
	centerlinesFilter->SetResamplingStepLength(1);
	centerlinesFilter->SetRadiusArrayName("Radius");
	centerlinesFilter->SetEdgeArrayName("Edge");
	centerlinesFilter->SetEdgePCoordArrayName("PCoord");
	centerlinesFilter->Update();

	m_centerline->DeepCopy(centerlinesFilter->GetOutput());
	//m_centerline->GetPointData()->SetScalars(m_centerline->GetPointData()->GetArray(0));
}

void Centerline::GetSurface(vtkPolyData* surface)
{
	surface = m_surface;
}

void Centerline::GetCappedSurface(vtkPolyData* cappedSurface)
{
	cappedSurface->DeepCopy(m_cappedSurface);
}

void Centerline::GetCenterline(vtkPolyData* centerline)
{
	centerline->DeepCopy(m_centerline);
}

vtkPolyData* Centerline::GetCenterline()
{
	return m_centerline;
}

//void Centerline::CreateLookupTable()
//{
//	m_LUT->SetTableRange(m_centerline->GetScalarRange());
//	m_LUT->Build();
//	m_LUT->SetTableValue(0, 1.0, 0.0, 0.0, 1);  //Red
//	m_LUT->SetTableValue(1, 0.0, 1.0, 0.0, 1); // Green
//	m_LUT->SetTableValue(2, 0.0, 0.0, 1.0, 1); // Blue
//}
//
//void Centerline::SetLookupTable(vtkLookupTable* LUT)
//{
//	m_LUT = LUT;
//}
//
//void Centerline::GetLookupTable(vtkLookupTable* LUT)
//{
//	LUT->DeepCopy(m_LUT);
// }