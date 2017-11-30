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

// The mouse motion callback, to turn "Slicing" on and off
class vtkImageInteractionCallback : public vtkCommand
{
public:

    static vtkImageInteractionCallback *New() {
        return new vtkImageInteractionCallback;
    };

    vtkImageInteractionCallback() {
        this->Slicing = 0;
        this->ImageReslice = 0;
        this->ComImageReslice = 0;
        this->Interactor = 0;
    };

    void SetImageReslice(vtkImageReslice *reslice1, vtkImageReslice *reslice2) {
        this->ImageReslice = reslice1;
        this->ComImageReslice = reslice2;
    };

    vtkImageReslice *GetImageReslice() {
        return this->ImageReslice;
    };

    void SetInteractor(vtkRenderWindowInteractor *interactor) {
        this->Interactor = interactor;
    };

    vtkRenderWindowInteractor *GetInteractor() {
        return this->Interactor;
    };

    void Execute(vtkObject *, unsigned long event, void *) VTK_OVERRIDE
    {
        vtkRenderWindowInteractor *interactor = this->GetInteractor();

        int lastPos[2];
        interactor->GetLastEventPosition(lastPos);
        int currPos[2];
        interactor->GetEventPosition(currPos);

        if (event == vtkCommand::LeftButtonPressEvent)
        {
            this->Slicing = 1;
        }
        else if (event == vtkCommand::LeftButtonReleaseEvent)
        {
            this->Slicing = 0;
        }
        else if (event == vtkCommand::MouseMoveEvent)
        {
            if (this->Slicing)
            {
                vtkImageReslice *reslice = this->ImageReslice;

                // Increment slice position by deltaY of mouse
                int deltaY = lastPos[1] - currPos[1];

                reslice->Update();
                double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
                vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
                // move the center point that we are slicing through
                double point[4];
                double center[4];
                point[0] = 0.0;
                point[1] = 0.0;
                point[2] = sliceSpacing * deltaY;
                point[3] = 1.0;
                matrix->MultiplyPoint(point, center);
                matrix->SetElement(0, 3, center[0]);
                matrix->SetElement(1, 3, center[1]);
                matrix->SetElement(2, 3, center[2]);

                this->ComImageReslice->Update();
                vtkMatrix4x4 *matrix2 = this->ComImageReslice->GetResliceAxes();
                matrix2->SetElement(0, 3, center[0]);
                matrix2->SetElement(1, 3, center[1]);
                matrix2->SetElement(2, 3, center[2]);
                interactor->Render();
            }
            else
            {
                vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
                    interactor->GetInteractorStyle());
                if (style)
                {
                    style->OnMouseMove();
                }
            }
        }
    };

private:

    // Actions (slicing only, for now)
    int Slicing;

    // Pointer to vtkImageReslice
    vtkImageReslice *ImageReslice;

    vtkImageReslice *ComImageReslice;

    // Pointer to the interactor
    vtkRenderWindowInteractor *Interactor;
};

int main()
{
   // TestLoadDoseImage();
    TestBlendCTImageAndDose();
    return 0;
}

void TestBlendCTImageAndDose()
{
    std::string sImagePath = "D:\\GitHub\\WisdomRay\\test\\ctimage.dat";
    int iImageVolumeDimension[3] = { 512, 512, 198 };
    int iImageVolumeComponent = 1;
    int iRescaleSlope = 1;
    int iRescaleIntercept = -1024;
    vtkSmartPointer<vtkImageData> ctImageData = vtkSmartPointer<vtkImageData>::New();
    ctImageData->SetDimensions(iImageVolumeDimension);
    ctImageData->AllocateScalars(VTK_SHORT, iImageVolumeComponent);
    ctImageData->SetOrigin(0, 0, 0);
    ctImageData->SetSpacing(1.26953125, 1.26953125, 2);
    short* imageVolumeData = LoadRawVolume<short>(iImageVolumeDimension, sImagePath, iImageVolumeComponent);
    short* imagePtr = reinterpret_cast<short*>(ctImageData->GetScalarPointer());
    for (int i = 0; i < iImageVolumeDimension[0] * iImageVolumeDimension[1] * iImageVolumeDimension[2] * iImageVolumeComponent; i += iImageVolumeComponent)
    {
        imagePtr[i] = imageVolumeData[i] * iRescaleSlope + iRescaleIntercept;
    }

    double spacing[3];
    double origin[3];
    int* extent = ctImageData->GetExtent();
    ctImageData->GetSpacing(spacing);
    ctImageData->GetOrigin(origin);
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    static double axialElements[16] = {
        1,  0,  0,0,
        0,  0,  -1, 0,
        0,  -1,  0, 0,
        0,  0,  0, 1 };

    // Set the slice orientation
    vtkSmartPointer<vtkMatrix4x4> resliceAxes =
        vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxes->DeepCopy(axialElements);
    // Set the point through which to slice
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> reslice =
        vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputData(ctImageData);
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();

    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> table =
        vtkSmartPointer<vtkLookupTable>::New();
    table->SetRange(-300, 350); // image intensity range
    table->SetValueRange(0.0, 1.0); // from black to white
    table->SetSaturationRange(0.0, 0.0); // no color saturation
    table->SetRampToLinear();
    table->Build();

    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> color =
        vtkSmartPointer<vtkImageMapToColors>::New();
    color->SetLookupTable(table);
    color->SetInputConnection(reslice->GetOutputPort());
    
    //DOSE信息
    std::string fDoseFilePath = "D:\\GitHub\\WisdomRay\\appdata\\dose.raw";
    int iVolumeDimension[3] = { 150, 138, 198 };
    int iComponent = 1;
    unsigned int* imageData = LoadRawVolume<unsigned int>(iVolumeDimension, fDoseFilePath, iComponent);
    vtkSmartPointer<vtkImageData> doseImageData = vtkSmartPointer<vtkImageData>::New();
    doseImageData->SetDimensions(iVolumeDimension);
    doseImageData->AllocateScalars(VTK_DOUBLE, iComponent);
    doseImageData->SetOrigin(0, 0, 0);
    doseImageData->SetSpacing(2.5390625, 2.5390625, 2);
    double* ptr = reinterpret_cast<double*>(doseImageData->GetScalarPointer());
    for (int i = 0; i < iVolumeDimension[0] * iVolumeDimension[1] * iVolumeDimension[2] * iComponent; i += iComponent)
    {
        ptr[i] = imageData[i] * 6.998e-5;
    }

    //体素中最值
    double scalarRange[2];
    doseImageData->GetScalarRange(scalarRange);

    // Set the slice orientation
    vtkSmartPointer<vtkMatrix4x4> resliceAxesTop = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesTop->DeepCopy(axialElements);
    resliceAxesTop->SetElement(0, 3, center[0]);
    resliceAxesTop->SetElement(1, 3, center[0]);
    resliceAxesTop->SetElement(2, 3, center[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceTop = vtkSmartPointer<vtkImageReslice>::New();
    resliceTop->SetInputData(doseImageData);
    resliceTop->SetOutputDimensionality(2);
    resliceTop->SetResliceAxes(resliceAxesTop);
    resliceTop->SetInterpolationModeToLinear();
    resliceTop->Update();

    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> tableTop = vtkSmartPointer<vtkLookupTable>::New();
    tableTop->SetNumberOfTableValues(6);
    tableTop->SetRange(scalarRange[0], scalarRange[1]);
    tableTop->SetTableValue(0, 0.517, 0.710, 0.694, 0.5);
    tableTop->SetTableValue(1, 0.517, 0.710, 0.694, 0.5);
    tableTop->SetTableValue(2, 0.765, 0.808, 0.572, 0.5);
    tableTop->SetTableValue(3, 0.086, 0.521, 0.149, 0.5);
    tableTop->SetTableValue(4, 0.580, 0.580, 0.141, 0.5);
    tableTop->SetTableValue(5, 0.721, 0.266, 0.027, 0.5);
    tableTop->Build();

    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> colorTop = vtkSmartPointer<vtkImageMapToColors>::New();
    colorTop->SetLookupTable(tableTop);
    colorTop->SetInputData(resliceTop->GetOutput());
    colorTop->Update();

    //下面是轮廓线
    vtkSmartPointer<vtkContourFilter> contour = vtkSmartPointer<vtkContourFilter>::New();
    contour->SetInputData(resliceTop->GetOutput());
    contour->SetNumberOfContours(5);
    contour->SetValue(0, 10);
    contour->SetValue(1, 24);
    contour->SetValue(2, 40);
    contour->SetValue(3, 55);
    contour->SetValue(4, 60);
    contour->Update();

    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> contourTop = vtkSmartPointer<vtkLookupTable>::New();
    contourTop->SetNumberOfTableValues(5);
    contourTop->SetRange(scalarRange[0], scalarRange[1]);
    contourTop->SetTableValue(0, 0.517, 0.710, 0.694, 0.5);
    contourTop->SetTableValue(1, 0.765, 0.808, 0.572, 0.5);
    contourTop->SetTableValue(2, 0.086, 0.521, 0.149, 0.5);
    contourTop->SetTableValue(3, 0.580, 0.580, 0.141, 0.5);
    contourTop->SetTableValue(4, 0.721, 0.266, 0.027, 0.5);
    contourTop->Build();

    vtkSmartPointer<vtkPolyDataMapper> contourMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contour->GetOutputPort());
    contourMapper->SetInterpolateScalarsBeforeMapping(1);
    contourMapper->UseLookupTableScalarRangeOn();
    contourMapper->SetLookupTable(contourTop);

    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(contourTop);
    scalarBar->SetTitle("Dose");

    double t1 = 324.365234375 - 189.1601543;
    double t2 = -1.0* (508.365234375 - 423.9414063);
    vtkSmartPointer<vtkMatrix4x4> contourToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    static double contourElements[16] = {
        1, 0, 0, t1,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    contourToRASMatrix->DeepCopy(contourElements);


    // Display the image
    vtkSmartPointer<vtkImageActor> actor =
        vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputConnection(color->GetOutputPort());

    vtkSmartPointer<vtkImageActor> actorTop =
        vtkSmartPointer<vtkImageActor>::New();
    actorTop->GetMapper()->SetInputConnection(colorTop->GetOutputPort());
    actorTop->SetUserMatrix(contourToRASMatrix);

    vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->SetUserMatrix(contourToRASMatrix);
    contourActor->GetProperty()->SetLineWidth(2);


    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->AddActor(actorTop);
    renderer->AddActor(contourActor);
    renderer->AddActor2D(scalarBar);


    vtkSmartPointer<vtkRenderWindow> window =
        vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->Render();

    // Set up the interaction
    vtkSmartPointer<vtkInteractorStyleImage> imageStyle =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetInteractorStyle(imageStyle);
    window->SetInteractor(interactor);
    window->Render();

    vtkSmartPointer<vtkImageInteractionCallback> callback =
        vtkSmartPointer<vtkImageInteractionCallback>::New();
    callback->SetImageReslice(reslice, resliceTop);
    callback->SetInteractor(interactor);

    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

    // Start interaction
    // The Start() method doesn't return until the window is closed by the user
    interactor->Start();
}





