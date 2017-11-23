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
#include "vtkImageCanvasSource2D.h"


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
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

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
    double centerBottom[3];
    centerBottom[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    centerBottom[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    centerBottom[2] = origin[2] + spacing[2] * 0.6 * (extent[4] + extent[5]);

    vtkSmartPointer<vtkMatrix4x4> resliceAxesBottom =
        vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesBottom->DeepCopy(axialElements);
    resliceAxesBottom->SetElement(0, 3, centerBottom[0]);
    resliceAxesBottom->SetElement(1, 3, centerBottom[1]);
    resliceAxesBottom->SetElement(2, 3, centerBottom[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceBottom =
        vtkSmartPointer<vtkImageReslice>::New();
    resliceBottom->SetInputConnection(reader->GetOutputPort());
    resliceBottom->SetOutputDimensionality(2);
    resliceBottom->SetResliceAxes(resliceAxesBottom);
    resliceBottom->SetInterpolationModeToLinear();

    vtkSmartPointer<vtkLookupTable> tableBottom = vtkSmartPointer<vtkLookupTable>::New();
    tableBottom->SetRange(-355, 300); // image intensity range
    tableBottom->SetValueRange(0.0, 1.0); // from black to white
    tableBottom->SetSaturationRange(0.0, 0.0); // no color saturation
    tableBottom->SetRampToLinear();
    tableBottom->Build();

    vtkSmartPointer<vtkImageMapToColors> mapBottom = vtkSmartPointer<vtkImageMapToColors>::New();
    mapBottom->SetInputConnection(resliceBottom->GetOutputPort());
    mapBottom->SetLookupTable(tableBottom);
    mapBottom->Update();

    std::string fDoseFilePath = "D:\\GitHub\\WisdomRay\\appdata\\dose.dat";
    int iVolumeDimension[3] = { 150, 138, 198 };
    int iComponent = 4;
    unsigned char* imageData = LoadRawVolume<unsigned char>(iVolumeDimension, fDoseFilePath);
    vtkSmartPointer<vtkImageData> doseImageData = vtkSmartPointer<vtkImageData>::New();
    doseImageData->SetDimensions(iVolumeDimension);
    doseImageData->AllocateScalars(VTK_UNSIGNED_CHAR, iComponent);
    doseImageData->SetOrigin(0, 0, 0);
    doseImageData->SetSpacing(2.5390625, 2.5390625, 2);
    unsigned char* ptr = reinterpret_cast<unsigned char*>(doseImageData->GetScalarPointer());
    for (int i = 0; i < iVolumeDimension[0] * iVolumeDimension[1] * iVolumeDimension[2] * iComponent; i += iComponent)
    {
        ptr[i] = imageData[i];
        ptr[i + 1] = imageData[i + 1];
        ptr[i + 2] = imageData[i + 2];
        //ptr[i + 3] = imageData[i + 3];
    }

    vtkSmartPointer<vtkMatrix4x4> resliceAxesTop = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesTop->DeepCopy(axialElements);
    resliceAxesTop->SetElement(0, 3, centerBottom[0]);
    resliceAxesTop->SetElement(1, 3, centerBottom[1]);
    resliceAxesTop->SetElement(2, 3, centerBottom[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceTop = vtkSmartPointer<vtkImageReslice>::New();
    resliceTop->SetInputData(doseImageData);
    resliceTop->SetOutputDimensionality(2);
    resliceTop->SetResliceAxes(resliceAxesTop);
    resliceTop->SetInterpolationModeToLinear();

    vtkSmartPointer<vtkLookupTable> tableTop =  vtkSmartPointer<vtkLookupTable>::New();
    tableTop->SetTableRange(0, 1);
    tableTop->SetValueRange(1.0, 0.7);
    tableTop->SetSaturationRange(0.0, 1.0);
    tableTop->SetHueRange(0.12, 0.12);
    tableTop->SetAlphaRange(1.0, 1.0);
    tableTop->Build();

    vtkSmartPointer<vtkImageMapToColors> mapTop = vtkSmartPointer<vtkImageMapToColors>::New();
    mapTop->SetInputConnection(resliceTop->GetOutputPort());
    mapTop->SetLookupTable(tableTop);
    mapTop->Update();

    vtkSmartPointer<vtkImageCanvasSource2D> drawing = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    drawing->SetNumberOfScalarComponents(4);
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetExtent(extent);
    drawing->SetDrawColor(0.0, 0.0, 0.0);
    drawing->FillBox(extent[0], extent[1], extent[2], extent[3]);
    //drawing->SetDrawColor(255.0, 255.0, 255.0);
    drawing->DrawImage(508.365234375 - 423.9414063, 324.365234375 - 189.1601563,  mapTop->GetOutput());
    drawing->Update();


    vtkSmartPointer<vtkImageBlend> blend = vtkSmartPointer<vtkImageBlend>::New();
    blend->SetBlendModeToCompound();
    blend->AddInputData(0, mapBottom->GetOutput());
    blend->AddInputData(0, drawing->GetOutput());
    blend->SetOpacity(0, 0.5);
    blend->SetOpacity(1, 0.5);

    // Display the image
    vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
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




