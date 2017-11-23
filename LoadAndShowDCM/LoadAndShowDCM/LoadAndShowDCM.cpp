// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*=========================================================================

Program:   Visualization Toolkit
Module:    ImageSlicing.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example shows how to load a 3D image into VTK and then reformat
// that image into a different orientation for viewing.  It uses
// vtkImageReslice for reformatting the image, and uses vtkImageActor
// and vtkInteractorStyleImage to display the image.  This InteractorStyle
// forces the camera to stay perpendicular to the XY plane.
//
// Thanks to David Gobbi of Atamai Inc. for contributing this example.
//

#include "vtkSmartPointer.h"
#include "vtkImageReader2.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkImageGridSource.h"
#include "vtkImageBSplineInterpolator.h"
#include "vtkImageBSplineCoefficients.h"
#include "vtkImageReslice.h"
#include "vtkBSplineTransform.h"
#include "vtkTransformToGrid.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkImageViewer.h"
#include "vtkImageGridSource.h"
#include "vtkImageBlend.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRegressionTestImage.h"
#include "vtkSmartPointer.h"

namespace
{
    template<typename T>
    T* LoadRawVolume(int(&iDim)[3], const std::string &strPath)
    {
        std::ifstream in(strPath, std::ios::binary | std::ios::in);
        if (!in.is_open())
        {
            std::cout << "Can't open file : " << strPath << std::endl;
            return nullptr;
        }

        T* pData = new T[4 * iDim[0] * iDim[1] * iDim[2]];
        in.read((char*)pData, 4 * iDim[0] * iDim[1] * iDim[2] * sizeof(T));
        in.close();

        return pData;
    }
}

int main()
{
    std::string fDoseFilePath = "D:\\GitHub\\WisdomRay\\appdata\\dose.dat";
    int iVolumeDimension[3] = { 150, 138, 198 };
    int iComponent = 4;
    unsigned char* imageData = LoadRawVolume<unsigned char>(iVolumeDimension, fDoseFilePath);
    vtkSmartPointer<vtkImageData> doseImageData = vtkSmartPointer<vtkImageData>::New();
    doseImageData->SetDimensions(iVolumeDimension);
    doseImageData->AllocateScalars(VTK_UNSIGNED_CHAR, iComponent);
    doseImageData->SetOrigin(100, 100, 0);
    doseImageData->SetSpacing(2.5, 2.5, 1);
    unsigned char* ptr = reinterpret_cast<unsigned char*>(doseImageData->GetScalarPointer());
    for (int i = 0; i < iVolumeDimension[0] * iVolumeDimension[1] * iVolumeDimension[2] * iComponent; i += iComponent)
    {
        ptr[i] = imageData[i];
        ptr[i + 1] = imageData[i + 1];
        ptr[i + 2] = imageData[i + 2];
        ptr[i + 3] = imageData[i + 3];
    }


    std::string sPath = "E:\\Images\\Test\\";
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDataByteOrderToLittleEndian();
    reader->SetDirectoryName(sPath.c_str());
    reader->Update();
    int extent[6];
    double spacing[3];
    double origin[3];
    reader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);

    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };
    vtkSmartPointer<vtkMatrix4x4> resliceAxes =
        vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxes->DeepCopy(axialElements);
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> reslice =
        vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputConnection(reader->GetOutputPort());
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();

    vtkSmartPointer<vtkLookupTable> table2 =
        vtkSmartPointer<vtkLookupTable>::New();
    table2->SetRange(-355, 300); // image intensity range
    table2->SetValueRange(0.0, 1.0); // from black to white
    table2->SetSaturationRange(0.0, 0.0); // no color saturation
    table2->SetRampToLinear();
    table2->Build();


    vtkSmartPointer<vtkImageMapToColors> map2 =
        vtkSmartPointer<vtkImageMapToColors>::New();
    map2->SetInputConnection(reslice->GetOutputPort());
    map2->SetLookupTable(table2);
    map2->Update();


    vtkSmartPointer<vtkLookupTable> table1 =
        vtkSmartPointer<vtkLookupTable>::New();
    table1->SetTableRange(0, 1);
    table1->SetValueRange(1.0, 0.7);
    table1->SetSaturationRange(0.0, 1.0);
    table1->SetHueRange(0.12, 0.12);
    table1->SetAlphaRange(1.0, 1.0);
    table1->Build();

    vtkSmartPointer<vtkImageMapToColors> map1 =
        vtkSmartPointer<vtkImageMapToColors>::New();
    map1->SetInputData(doseImageData);
    map1->SetLookupTable(table1);
   map1->Update();

    vtkSmartPointer<vtkImageBlend> blend =
        vtkSmartPointer<vtkImageBlend>::New();
    blend->AddInputData(map2->GetOutput());
    blend->AddInputData(map1->GetOutput());
    blend->SetOpacity(0, 0.5);
    blend->SetOpacity(1, 0.5);


    // Display the image
    vtkSmartPointer<vtkImageActor> actor =
        vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputConnection(blend->GetOutputPort());

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> window =
        vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);

    // Set up the interaction
    vtkSmartPointer<vtkInteractorStyleImage> imageStyle =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetInteractorStyle(imageStyle);
    window->SetInteractor(interactor);
    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
