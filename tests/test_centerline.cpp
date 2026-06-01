#include <iostream>
#include <string>
#include <cassert>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkPointData.h>

#include "Centerline.h"

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "test_data"
#endif

static void check(bool condition, const std::string& msg)
{
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: test_centerline <left|right>" << std::endl;
        return 1;
    }

    std::string side = argv[1];
    if (side != "left" && side != "right") {
        std::cerr << "ERROR: side must be 'left' or 'right'" << std::endl;
        return 1;
    }

    std::string dataDir = std::string(TEST_DATA_DIR) + "/" + side;
    std::string lumenPath = dataDir + "/" + side + "lumen.stl";

    std::cout << "Testing centerline extraction for: " << side << std::endl;
    std::cout << "Loading: " << lumenPath << std::endl;

    // Load the open vessel surface
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(lumenPath.c_str());
    reader->Update();

    vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();
    surface->DeepCopy(reader->GetOutput());

    check(surface->GetNumberOfPoints() > 0, "Surface has no points — file load failed");
    std::cout << "Surface loaded: " << surface->GetNumberOfPoints() << " points" << std::endl;

    // Run centerline extraction (headless — no render window)
    Centerline centerlineExtractor;
    centerlineExtractor.SetSurface(surface);
    centerlineExtractor.Update();

    vtkPolyData* centerline = centerlineExtractor.GetCenterline();

    check(centerline != nullptr, "Centerline output is null");
    check(centerline->GetNumberOfPoints() > 0, "Centerline has no points");

    std::cout << "Centerline points: " << centerline->GetNumberOfPoints() << std::endl;

    // Check Radius array — set by vtkvmtkPolyDataCenterlines
    vtkDataArray* radiusArray = centerline->GetPointData()->GetArray("Radius");
    check(radiusArray != nullptr, "Radius array not found in centerline output");
    std::cout << "Radius array found with " << radiusArray->GetNumberOfTuples() << " tuples" << std::endl;

    std::cout << "PASS: " << side << " centerline test" << std::endl;
    return 0;
}
