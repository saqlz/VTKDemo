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
    return EXIT_SUCCESS;
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
    for (int i = iVolumeDimension[0] * iVolumeDimension[1] * 94; i < iVolumeDimension[0] * iVolumeDimension[1] * 95 * iComponent; i += iComponent)
    {
        double val = imageData[i] * 6.998e-5;
        ptr[i - iVolumeDimension[0] * iVolumeDimension[1] * 94] = val;
    }

    vtkSmartPointer<vtkContourFilter> surface = vtkSmartPointer<vtkContourFilter>::New();
    surface->SetInputData(doseImageData);
    surface->ComputeNormalsOn();
    surface->GenerateValues(5, 25, 55);
    surface->Update();

    double scalarRange[2];
    surface->GetOutput()->GetScalarRange(scalarRange);
    vtkSmartPointer<vtkAppendPolyData> appendFilledContours = vtkSmartPointer<vtkAppendPolyData>::New();
    int numberOfContours = 5;
    double delta = (scalarRange[1] - scalarRange[0]) / static_cast<double> (numberOfContours - 1);
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersHi;

    for (int i = 0; i < numberOfContours; i++)
    {
        double valueLo = scalarRange[0] + static_cast<double> (i) * delta;
        double valueHi = scalarRange[0] + static_cast<double> (i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
        {
            clippersLo[i]->SetInputData(doseImageData);
        }
        else
        {
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        }
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();
        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
        {
            continue;
        }

        vtkSmartPointer<vtkFloatArray> cd =  vtkSmartPointer<vtkFloatArray>::New();
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
    }

    vtkSmartPointer<vtkCleanPolyData> filledContours =
        vtkSmartPointer<vtkCleanPolyData>::New();
    filledContours->SetInputConnection(surface->GetOutputPort());
    filledContours->Update();

    vtkSmartPointer<vtkLookupTable> lut =
        vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(5);
    lut->SetTableValue(0, 0.517, 0.710, 0.694, 1.0);
    lut->SetTableValue(1, 0.765, 0.808, 0.572, 1.0);
    lut->SetTableValue(2, 0.086, 0.521, 0.149, 1.0);
    lut->SetTableValue(3, 0.580, 0.580, 0.141, 1.0);
    lut->SetTableValue(4, 0.721, 0.266, 0.027, 1.0);
    lut->Build();
    vtkSmartPointer<vtkPolyDataMapper> contourMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(filledContours->GetOutputPort());
    contourMapper->SetScalarRange(scalarRange[0], scalarRange[1]);
    contourMapper->SetLookupTable(lut);

    vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetInterpolationToFlat();
    vtkSmartPointer<vtkImageActor> doseActor = vtkSmartPointer<vtkImageActor>::New();
    doseActor->SetInputData(doseImageData);

    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(contourMapper->GetLookupTable());
    scalarBar->SetTitle("Test");

    // The usual renderer, render window and interactor
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    ren1->SetBackground(0,0,0);
    renWin->AddRenderer(ren1);
    iren->SetRenderWindow(renWin);

    // Add the actors
    ren1->AddActor(doseActor);
    ren1->AddActor(contourActor);
    ren1->AddActor2D(scalarBar);

    // Begin interaction
    renWin->Render();
    iren->Start();

}
