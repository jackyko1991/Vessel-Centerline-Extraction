#include <iostream>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkUnstructuredGridVolumeMapper.h>
#include <vtkProperty.h>
#include <vtkIdList.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkPNGWriter.h>

#include <vtkvmtkCenterlineAttributesFilter.h>
#include <vtkvmtkCenterlineBranchExtractor.h>
#include <vtkvmtkCenterlineBranchGeometry.h>
#include <vtkvmtkPolyDataCenterlineGroupsClipper.h>
#include <vtkvmtkCenterlineBifurcationReferenceSystems.h>
#include <vtkvmtkPolyDataCenterlineAngularMetricFilter.h>
#include <vtkvmtkPolyDataCenterlineAbscissaMetricFilter.h>
#include <vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter.h>
#include <vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter.h>
#include <vtkvmtkPolyDataStretchMappingFilter.h>
#include <vtkvmtkCapPolyData.h>
#include <vtkvmtkPolyDataPatchingFilter.h>
#include <vtkvmtkCenterlineGeometry.h>

#include <QFileInfo>
#include <QDir>

#include "interactorStyleCenterline.h"
#include "Centerline.h"
#include "NonManifoldSurfaceChecker.h"

using namespace std;

int WriteCenterline(vtkPolyData* centerline, string centerlinePath)
{
	QFileInfo centerlinePathInfo(centerlinePath.c_str());
	QDir centerlineDir = centerlinePathInfo.absoluteDir();

	if (centerlineDir.exists() &&
		(centerlinePathInfo.suffix().toStdString() == "vtp" ||
			centerlinePathInfo.suffix().toStdString() == "VTP" ||
			centerlinePathInfo.suffix().toStdString() == "vtk" ||
			centerlinePathInfo.suffix().toStdString() == "VTK"))
	{

		// compute centerline attribute, geometry and branch splitting
		vtkSmartPointer<vtkvmtkCenterlineAttributesFilter> attributeFilter = vtkSmartPointer<vtkvmtkCenterlineAttributesFilter>::New();
		attributeFilter->SetInputData(centerline);
		attributeFilter->SetParallelTransportNormalsArrayName("ParallelTransportNormals");
		attributeFilter->SetAbscissasArrayName("Abscissas");
		attributeFilter->Update();

		vtkSmartPointer<vtkvmtkCenterlineGeometry> geometryFilter = vtkSmartPointer<vtkvmtkCenterlineGeometry>::New();
		geometryFilter->SetInputData(attributeFilter->GetOutput());
		geometryFilter->SetFrenetBinormalArrayName("FrenetBinormal");
		geometryFilter->SetFrenetNormalArrayName("FrenetNormal");
		geometryFilter->SetFrenetTangentArrayName("FrenetTangent");
		geometryFilter->SetLengthArrayName("Length");
		geometryFilter->SetTorsionArrayName("Torsion");
		geometryFilter->SetTortuosityArrayName("Tortuosity");
		geometryFilter->SetCurvatureArrayName("Curvature");
		geometryFilter->SetLineSmoothing(0);
		geometryFilter->SetNumberOfSmoothingIterations(100);
		geometryFilter->SetSmoothingFactor(0.1);
		geometryFilter->SetGlobalWarningDisplay(1);
		geometryFilter->Update();

		vtkSmartPointer<vtkvmtkCenterlineBranchExtractor> branchExtractor = vtkSmartPointer<vtkvmtkCenterlineBranchExtractor>::New();
		branchExtractor->SetInputData(geometryFilter->GetOutput());
		branchExtractor->SetRadiusArrayName("Radius");
		branchExtractor->SetCenterlineIdsArrayName("CenterlineIds");
		branchExtractor->SetGroupIdsArrayName("GroupIds");
		branchExtractor->SetBlankingArrayName("Blanking");
		branchExtractor->SetTractIdsArrayName("TractIds");
		branchExtractor->Update();

		centerline->DeepCopy(branchExtractor->GetOutput());

		//	vtkSmartPointer<vtkvmtkCenterlineBranchGeometry> branchGeometry = vtkSmartPointer<vtkvmtkCenterlineBranchGeometry>::New();
		//	branchGeometry->SetInputData(branchExtractor->GetOutput());
		//	branchGeometry->SetRadiusArrayName("Radius");
		//	branchGeometry->SetGroupIdsArrayName("GroupIds");
		//	branchGeometry->SetBlankingArrayName("Blanking");
		//	branchGeometry->SetCurvatureArrayName("Curvature");
		//	branchGeometry->SetLengthArrayName("Length");
		//	branchGeometry->SetTorsionArrayName("Torsion");
		//	branchGeometry->SetTortuosityArrayName("Tortuosity");
		//	branchGeometry->SetLineSmoothing(0);
		//	branchGeometry->SetNumberOfSmoothingIterations(100);
		//	branchGeometry->SetSmoothingFactor(0.1);
		//	branchGeometry->SetGlobalWarningDisplay(1);
		//	branchGeometry->Update();

		vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
		writer->SetInputData(branchExtractor->GetOutput());
		writer->SetFileName(centerlinePath.c_str());
		writer->Write();
		cout << "Successfully write the centerline polydata file" << endl;
		return 1;
	}
	else
	{
		cout << "Invalid centerline save path/ save file type not support" << endl;
		return 0;
	}

	
}

int ReadSurface(string surfacePath, vtkPolyData* surface)
{
	QFileInfo inputSurfaceInfo(surfacePath.c_str());
	cout << "surface file: " << surfacePath << endl;

	if (inputSurfaceInfo.suffix().toStdString() == "stl" || inputSurfaceInfo.suffix().toStdString() == "STL")
	{
		// read surface
		vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(surfacePath.c_str());
		reader->Update();
		surface->DeepCopy(reader->GetOutput());
	}
	else if (inputSurfaceInfo.suffix().toStdString() == "vtp" || inputSurfaceInfo.suffix().toStdString() == "VTP")
	{
		vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
		reader->SetFileName(surfacePath.c_str());
		reader->Update();
		surface->DeepCopy(reader->GetOutput());
	}
	else if (inputSurfaceInfo.suffix().toStdString() == "vtk" || inputSurfaceInfo.suffix().toStdString() == "VTK")
	{
		vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
		reader->SetFileName(surfacePath.c_str());
		reader->Update();
		surface->DeepCopy( reader->GetOutput());
	}
	else
	{
		cerr << "File type not support" << endl;
		return 0;
	}

	// preprocess the surface
	vtkSmartPointer<vtkCleanPolyData>surfaceCleaner = vtkSmartPointer<vtkCleanPolyData>::New();
	surfaceCleaner->SetInputData(surface);
	surfaceCleaner->Update();

	vtkSmartPointer < vtkTriangleFilter> surfaceTriangulator = vtkSmartPointer<vtkTriangleFilter>::New();
	surfaceTriangulator->SetInputConnection(surfaceCleaner->GetOutputPort());
	surfaceTriangulator->PassLinesOff();
	surfaceTriangulator->PassVertsOff();
	surfaceTriangulator->Update();

	surface->DeepCopy(surfaceTriangulator->GetOutput());
	return 1;
}

int ExtractCenterline(string surfacePath, vtkPolyData* inputSurface, vtkPolyData* cappedSurface, vtkPolyData* centerline, bool &cylindricalShape)
{
	QFileInfo inputSurfaceInfo(surfacePath.c_str());

	if (inputSurfaceInfo.exists())
	{
		// read surface file
		if (!ReadSurface(surfacePath.c_str(), inputSurface))
			return 0;
		else
		{
			// capping
			vtkSmartPointer<vtkvmtkCapPolyData> capper = vtkSmartPointer<vtkvmtkCapPolyData>::New();
			capper->SetInputData(inputSurface);
			capper->Update();

			vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
			cleaner->SetInputData(capper->GetOutput());
			cleaner->Update();
			
			vtkSmartPointer<vtkTriangleFilter> triangulator = vtkSmartPointer<vtkTriangleFilter>::New();
			triangulator->SetInputData(cleaner->GetOutput());
			triangulator->PassLinesOff();
			triangulator->PassVertsOff();
			triangulator->Update();
			
			cappedSurface->DeepCopy(triangulator->GetOutput());			

			vtkSmartPointer<vtkPolyDataMapper> surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			surfaceMapper->SetInputData(cappedSurface);
			vtkSmartPointer<vtkActor> surfaceActor = vtkSmartPointer<vtkActor>::New();
			surfaceActor->SetMapper(surfaceMapper);

			vtkSmartPointer<vtkPolyDataMapper> centerlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			centerlineMapper->SetInputData(centerline);
			vtkSmartPointer<vtkActor> centerlineActor = vtkSmartPointer<vtkActor>::New();
			centerlineActor->SetMapper(centerlineMapper);

			// put the actor into render window
			vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
			ren->AddActor(surfaceActor);
			ren->AddActor(centerlineActor);

			vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
			renWin->AddRenderer(ren);

			vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
			vtkSmartPointer<MouseInteractorStyleCenterline> style = vtkSmartPointer<MouseInteractorStyleCenterline>::New();
			iren->SetInteractorStyle(style);
			iren->SetRenderWindow(renWin);
			style->SetSurface(cappedSurface);
			style->SetCenterline(centerline);

			if (capper->GetCapCenterIds()->GetNumberOfIds() >= 2)
			{
				style->SetAppendEndPoints(1);
				cylindricalShape = 1;
			}
			else
			{
				cylindricalShape = 0;
			}
				

			iren->Initialize();
			renWin->Render();
			ren->ResetCamera();
			iren->Start();

			return 1;
		}
	}
	else
		return 0;
}

inline bool fileExists(const std::string& name) 
{
	ifstream f(name.c_str());
	return f.good();
}

int WriteCappedSurface(vtkPolyData* cappedSurface, string writePath)
{
	vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
	writer->SetFileName(writePath.c_str());
	writer->SetInputData(cappedSurface);
	writer->Write();
	cout << "Successfully write the capped surface file"<<endl;
	return 1;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Usage: Centerline.exe surface.stl capped_surface.stl centerline.vtp" << endl;
		return 0;
	}
	else
	{
		cout << "Centerline extraction in progress..." << endl;

		vtkSmartPointer<vtkPolyData> centerline = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> inputSurface = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> cappedSurface = vtkSmartPointer<vtkPolyData>::New();
		bool cylindricalShape;
		if (!ExtractCenterline(argv[1], inputSurface,cappedSurface,centerline, cylindricalShape))
			return 0;
		else
		{

			cout << "Finish Centerline extraction" << endl;
			if (argc == 4)
			{
				// write the capped file
				if (!WriteCappedSurface(cappedSurface, argv[2]))
					return 0;					
				
				if (!WriteCenterline(centerline, argv[3]))
					return 0;
			}
		}
		//	

		//system("pause");
	}
}
