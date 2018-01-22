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
#include "vtkImageData.h"
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>


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
    // Matrices for axial, coronal, sagittal, oblique view orientations
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    std::string sPath = "E:\\Images\\TestImageReg\\ImageReg__20180117124908_CT\\";
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
    center[0] = origin[0];
    center[1] = origin[1];
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);



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
    reslice->SetInputData(reader->GetOutput());
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
    std::string sPathTop = "E:\\Images\\TestImageReg\\ImageReg__20180117124908_MR\\";
    vtkSmartPointer<vtkDICOMImageReader> readerTop = vtkSmartPointer<vtkDICOMImageReader>::New();
    readerTop->SetDataByteOrderToLittleEndian();
    readerTop->SetDirectoryName(sPathTop.c_str());
    readerTop->Update();

    vtkSmartPointer<vtkImageBlend> imgBlender =
        vtkSmartPointer<vtkImageBlend>::New();
    imgBlender->SetOpacity(0, 0.5);
    imgBlender->SetOpacity(1, 0.5);
    imgBlender->AddInputConnection(reader->GetOutputPort());
    imgBlender->AddInputConnection(readerTop->GetOutputPort());
   
    vtkSmartPointer<vtkDataSetMapper> imgDataSetMapper =
        vtkSmartPointer<vtkDataSetMapper>::New();
    imgDataSetMapper->SetInputConnection(imgBlender->GetOutputPort());

    vtkSmartPointer<vtkActor> imgActor =
        vtkSmartPointer<vtkActor>::New();
    imgActor->SetMapper(imgDataSetMapper);





    //体素中最值
    double scalarRange[2];
    readerTop->GetOutput()->GetScalarRange(scalarRange);

    static double axialElements1[16] = {
        0.9943, -0.1064, -0.0076, 61.2868,
        0.1049, 0.9625,  0.2503, -145.8125,
        -0.0193,-0.2497, 0.9681, -59.5743,
        0, 0, 0, 1 };

    static double axialElements2[16] = {
        0.9943, 0.1049, -0.0193,  61.2868,
        -0.1064, 0.9625, -0.2497,  -145.8125,
        -0.0076,0.2503, 0.9681,-59.5743,
        0, 0, 0, 1 };


    // Set the slice orientation
    vtkSmartPointer<vtkMatrix4x4> resliceAxesTop = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesTop->DeepCopy(axialElements1);
    resliceAxesTop->SetElement(0, 3, center[0]);
    resliceAxesTop->SetElement(1, 3, center[1]);
    resliceAxesTop->SetElement(2, 3, center[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceTop = vtkSmartPointer<vtkImageReslice>::New();
    resliceTop->SetInputData(readerTop->GetOutput());
    resliceTop->SetOutputDimensionality(2);
    resliceTop->SetResliceAxes(resliceAxesTop);
    resliceTop->SetInterpolationModeToLinear();
    resliceTop->Update();

    vtkSmartPointer<vtkLookupTable> tableTop =
        vtkSmartPointer<vtkLookupTable>::New();
    tableTop->SetTableRange(scalarRange[0], scalarRange[1]);
    tableTop->SetValueRange(0.3, 0.8);
    tableTop->SetSaturationRange(0.0, 1.0);
    tableTop->SetHueRange(0.12, 0.12);
    tableTop->SetAlphaRange(0.8, 0.8);
    tableTop->Build();

    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> colorTop = vtkSmartPointer<vtkImageMapToColors>::New();
    colorTop->SetLookupTable(tableTop);
    colorTop->SetInputData(resliceTop->GetOutput());
    colorTop->Update();

    vtkSmartPointer<vtkMatrix4x4> contourToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    static double contourElements[16] = {
        1, 0, 0, 92,
        0, 1, 0, 124,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    contourToRASMatrix->DeepCopy(contourElements);


    // Display the image
    vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputConnection(color->GetOutputPort());

    vtkSmartPointer<vtkImageActor> actorTop = vtkSmartPointer<vtkImageActor>::New();
    actorTop->GetMapper()->SetInputConnection(colorTop->GetOutputPort());
   // actorTop->SetUserMatrix(contourToRASMatrix);


    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->AddActor(actorTop);
    renderer->AddActor(imgActor);
   // renderer->AddActor(contourActor);
   // renderer->AddActor2D(scalarBar);


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





