// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageViewer2.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
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
#include "vtkImageCanvasSource2D.h"
#include "vtkImageShrink3D.h" //降采样头文件  
#include "vtkImageMagnify.h" //升采样头文件 
#include "vtkMarchingCubes.h"

void TestLoadCTImage();
void TestLoadDoseImage();
void TestLoadCTImageReslice();
void TestBlendCTImageAndDose();
namespace
{
    template<typename T>
    T* LoadRawVolume(int(&iDim)[3], const std::string &strPath, const int num)
    {
        std::ifstream in(strPath, std::ios::binary | std::ios::in);
        if (!in.is_open())
        {
            std::cout << "Can't open file : " << strPath << std::endl;
            return nullptr;
        }

        T* pData = new T[num * iDim[0] * iDim[1] * iDim[2]];
        in.read((char*)pData, num * iDim[0] * iDim[1] * iDim[2] * sizeof(T));
        in.close();
        return pData;
    }
}

int main()
{
    TestLoadDoseImage();
    return 0;
}

void TestLoadDoseImage()
{
    std::string fDoseFilePath = "D:\\GitHub\\WisdomRay\\appdata\\dose.raw";
    int iVolumeDimension[3] = { 150, 138, 198 };
    int iComponent = 1;
    unsigned int* imageData = LoadRawVolume<unsigned int>(iVolumeDimension, fDoseFilePath, iComponent);
    vtkSmartPointer<vtkImageData> doseImageData = vtkSmartPointer<vtkImageData>::New();
    doseImageData->SetDimensions(150, 138, 1);
    doseImageData->AllocateScalars(VTK_DOUBLE, iComponent);
    doseImageData->SetOrigin(0, 0, 0);
    doseImageData->SetSpacing(2.5390625, 2.5390625, 0);
    double* ptr = reinterpret_cast<double*>(doseImageData->GetScalarPointer());
    for (int i = iVolumeDimension[0] * iVolumeDimension[1] * 94; i < iVolumeDimension[0] * iVolumeDimension[1] *95 * iComponent; i += iComponent)
    {
        double val = imageData[i] * 6.998e-5;
        ptr[i - iVolumeDimension[0] * iVolumeDimension[1] * 94] = val;
    }

    vtkSmartPointer<vtkContourFilter> surface = vtkSmartPointer<vtkContourFilter>::New();
    surface->SetInputData(doseImageData);
    surface->ComputeNormalsOn();
    surface->GenerateValues(5, 25, 55);
    surface->Update();

    auto InputPolyData = surface->GetOutput();
    vtkNew<vtkPolyDataNormals> normalFilter;
    normalFilter->SetInputData(InputPolyData);
    normalFilter->ConsistencyOn();

    // Make sure that we have a clean triangle polydata
    vtkNew<vtkTriangleFilter> triangle;
    triangle->SetInputConnection(normalFilter->GetOutputPort());
    triangle->Update();

    // Convert to triangle strip
    vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer<vtkStripper>::New();
    stripper->SetInputConnection(triangle->GetOutputPort());
    stripper->Update();

    int referenceExtents[6] = { 0,149,0,137,0,0 };
    double origin[3] = { 0,0,0 };
    vtkSmartPointer<vtkImageData> referenceImage = vtkSmartPointer<vtkImageData>::New();
    // Blank reference image
    referenceImage->SetExtent(referenceExtents);
    referenceImage->SetSpacing(2.5390625, 2.5390625, 0);
    referenceImage->SetOrigin(origin);
    referenceImage->AllocateScalars(VTK_DOUBLE, 1);
    double* refPtr = reinterpret_cast<double*>(referenceImage->GetScalarPointer());
    memset(refPtr, 0, ((referenceExtents[1] - referenceExtents[0] + 1)*(referenceExtents[3] - referenceExtents[2] + 1)*(referenceExtents[5] - referenceExtents[4] + 1)*referenceImage->GetScalarSize()*referenceImage->GetNumberOfScalarComponents()));

    // Convert polydata to stencil
    vtkNew<vtkPolyDataToImageStencil> polyToImage;
    polyToImage->SetInputConnection(stripper->GetOutputPort());
    polyToImage->SetOutputSpacing(2.5390625, 2.5390625, 0);
    polyToImage->SetOutputOrigin(origin);
    polyToImage->SetOutputWholeExtent(referenceExtents);
    polyToImage->Update();

    // Convert stencil to image
    vtkNew<vtkImageStencil> stencil;
    stencil->SetInputData(referenceImage);
    stencil->SetStencilData(polyToImage->GetOutput());
    stencil->ReverseStencilOn();
    stencil->SetBackgroundValue(0);
    stencil->Update();


    /*
        vtkSmartPointer<vtkLookupTable> pColorTable = vtkLookupTable::New();
        pColorTable->SetNumberOfTableValues(5);
        pColorTable->SetTableRange(10, 75);
        pColorTable->SetTableValue(0, 0.517, 0.710, 0.694, 1.0);
        pColorTable->SetTableValue(1, 0.765, 0.808, 0.572, 1.0);
        pColorTable->SetTableValue(2, 0.086, 0.521, 0.149, 1.0);
        pColorTable->SetTableValue(3, 0.580, 0.580, 0.141, 1.0);
        pColorTable->SetTableValue(4, 0.721, 0.266, 0.027, 1.0);
        pColorTable->Build();

        vtkSmartPointer<vtkPolyDataMapper> surfMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        surfMapper->SetInputData(surface->GetOutput());
        surfMapper->SetLookupTable(pColorTable);
        surfMapper->UseLookupTableScalarRangeOn();
        surfMapper->ScalarVisibilityOff();
        surfMapper->Update();

        vtkSmartPointer<vtkActor> surfActor = vtkSmartPointer<vtkActor>::New();
        surfActor->SetMapper(surfMapper);
        surfActor->GetProperty()->SetColor(0.517, 0.710, 0.694);
        surfActor->GetProperty()->SetLineWidth(2.0);
        vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(surfMapper->GetLookupTable());
        scalarBar->SetTitle("Test");
    */

    vtkNew<vtkImageCast> imageCast;
    imageCast->SetInputConnection(stencil->GetOutputPort());
    imageCast->SetOutputScalarTypeToDouble();
    imageCast->Update();
    auto OutputLabelmap = vtkSmartPointer<vtkImageData>::New();
    OutputLabelmap->ShallowCopy(imageCast->GetOutput());

    vtkSmartPointer<vtkImageActor> allActor = vtkSmartPointer<vtkImageActor>::New();
    allActor->SetInputData(OutputLabelmap);
    vtkSmartPointer<vtkImageActor> doseActor = vtkSmartPointer<vtkImageActor>::New();
    doseActor->SetInputData(doseImageData);
    vtkSmartPointer<vtkRenderer> surfRender = vtkSmartPointer<vtkRenderer>::New();
    surfRender->AddActor(allActor);
   // surfRender->AddActor(doseActor);
   // surfRender->AddActor2D(scalarBar);
    surfRender->SetBackground(0, 0, 0);

    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(surfRender);
    window->Render();
    vtkSmartPointer<vtkRenderWindowInteractor> rwi = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    window->SetInteractor(rwi);
    rwi->Start();
}
