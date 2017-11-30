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
    //TestLoadDoseImage();
    //TestLoadCTImage();
    //TestLoadCTImageReslice();
    TestBlendCTImageAndDose();
    return 0;
}

void TestBlendCTImageAndDose()
{
    // Matrices for axial, coronal, sagittal, oblique view orientations
    // Matrices for axial, coronal, sagittal, oblique view orientations
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    std::string sImagePath = "D:\\GitHub\\WisdomRay\\test\\ctimage.dat";
    int iImageVolumeDimension[3] = { 512, 512, 198 };
    int iImageVolumeComponent = 1;
    vtkSmartPointer<vtkImageData> ctImageData = vtkSmartPointer<vtkImageData>::New();
    ctImageData->SetDimensions(iImageVolumeDimension);
    ctImageData->AllocateScalars(VTK_SHORT, iImageVolumeComponent);
    ctImageData->SetOrigin(0, 0, 0);
    ctImageData->SetSpacing(1.26953125, 1.26953125, 2);
    short* imageVolumeData = LoadRawVolume<short>(iImageVolumeDimension, sImagePath, iImageVolumeComponent);
    short* imagePtr = reinterpret_cast<short*>(ctImageData->GetScalarPointer());
    for (int i = 0; i < iImageVolumeDimension[0] * iImageVolumeDimension[1] * iImageVolumeDimension[2] * iImageVolumeComponent; i += iImageVolumeComponent)
    {
        imagePtr[i] = imageVolumeData[i] * 1 - 1024;
    }

    double spacing[3];
    double origin[3];
    ctImageData->GetSpacing(spacing);
    ctImageData->GetOrigin(origin);
    int* bound = ctImageData->GetExtent();
    double imageCenter[3];
    imageCenter[0] = origin[0] + spacing[0] * 0.5 * (bound[0] + bound[1]);
    imageCenter[1] = origin[1] + spacing[1] * 0.5 * (bound[2] + bound[3]);
    imageCenter[2] = origin[2] + spacing[2] * 0.5 * (bound[4] + bound[5]);

    vtkSmartPointer<vtkMatrix4x4> resliceAxesBottom =
        vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesBottom->DeepCopy(axialElements);
    resliceAxesBottom->SetElement(0, 3, imageCenter[0]);
    resliceAxesBottom->SetElement(1, 3, imageCenter[1]);
    resliceAxesBottom->SetElement(2, 3, imageCenter[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceBottom =
        vtkSmartPointer<vtkImageReslice>::New();
    resliceBottom->SetInputData(ctImageData);
    resliceBottom->SetOutputDimensionality(2);
    resliceBottom->SetResliceAxes(resliceAxesBottom);
    resliceBottom->SetInterpolationModeToLinear();
    resliceBottom->Update();

    vtkSmartPointer<vtkLookupTable> tableBottom = vtkSmartPointer<vtkLookupTable>::New();
    tableBottom->SetRange(-300, 350); // image intensity range
    tableBottom->SetValueRange(0.0, 1.0); // from black to white
    tableBottom->SetSaturationRange(0.0, 0.0); // no color saturation
    tableBottom->SetRampToLinear();
    tableBottom->Build();

    vtkSmartPointer<vtkImageMapToColors> mapBottom = vtkSmartPointer<vtkImageMapToColors>::New();
    mapBottom->SetInputConnection(resliceBottom->GetOutputPort());
    mapBottom->SetLookupTable(tableBottom);
    mapBottom->Update();

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
    for (int i =0; i < iVolumeDimension[0] * iVolumeDimension[1] * iVolumeDimension[2] * iComponent; i += iComponent)
    {
        ptr[i] = imageData[i] * 6.998e-5;
    }
  
    // Set the slice orientation
    vtkSmartPointer<vtkMatrix4x4> resliceAxesTop = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesTop->DeepCopy(axialElements);
    resliceAxesTop->SetElement(0, 3, imageCenter[0]);
    resliceAxesTop->SetElement(1, 3, imageCenter[0]);
    resliceAxesTop->SetElement(2, 3, imageCenter[2]);

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
    tableTop->SetRange(0, 77);
    tableTop->SetTableValue(0, 0, 0, 0, 0);
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
    
    vtkSmartPointer<vtkContourFilter> contour = vtkSmartPointer<vtkContourFilter>::New();
    contour->SetInputData(resliceTop->GetOutput());
    contour->GenerateValues(5, 25, 55);
    contour->Update();

    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> contourTop = vtkSmartPointer<vtkLookupTable>::New();
    contourTop->SetNumberOfTableValues(5);
    contourTop->SetRange(25, 55);
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



    //////重采样
    //vtkSmartPointer<vtkImageMagnify> magnifyFilter = vtkSmartPointer<vtkImageMagnify>::New();
    //magnifyFilter->SetInputData(colorTop->GetOutput());
    //magnifyFilter->SetMagnificationFactors(2.5390625 / spacing[0], 2.5390625 / spacing[0], 1);
    //magnifyFilter->InterpolateOn();
    //magnifyFilter->Update();

    // int originalDims[3];
    // reader->GetOutput()->GetDimensions(originalDims);
    // double originalSpacing[3];
    // reader->GetOutput()->GetSpacing(originalSpacing);
    // int magnifyDims[3];
    //// magnifyFilter->GetOutput()->GetDimensions(magnifyDims);
    // double magnifuSpacing[3];
    // //magnifyFilter->GetOutput()->GetSpacing(magnifuSpacing);
    // std::cout << "原始维度：      " << originalDims[0] << "*" << originalDims[1] << "*" << originalDims[2] << std::endl;
    // std::cout << "原始像素间距：  " << originalSpacing[0] << " " << originalSpacing[1] << " " << originalSpacing[2] << std::endl;
    // std::cout << "升采样维度：    " << magnifyDims[0] << "*" << magnifyDims[1] << "*" << magnifyDims[2] << std::endl;
    // std::cout << "升采样像素间距：" << magnifuSpacing[0] << " " << magnifuSpacing[1] << " " << magnifuSpacing[2] << std::endl;


    //vtkSmartPointer<vtkImageCanvasSource2D> drawing = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    //drawing->SetNumberOfScalarComponents(4);
    //drawing->SetScalarTypeToUnsignedChar();
    //drawing->SetExtent(0, 511, 0, 511, 0, 0);
    //drawing->DrawImage((324.365234375 - 189.1601543) / spacing[0], 512 - 138 * 2 - ((508.365234375 - 423.9414063) / spacing[1]), 
    //    colorTop->GetOutput());
    //drawing->Update();

    //vtkSmartPointer<vtkImageBlend> blend = vtkSmartPointer<vtkImageBlend>::New();
    //blend->SetBlendModeToCompound();
    //blend->AddInputData(mapBottom->GetOutput());
    //blend->AddInputData(drawing->GetOutput());
    //blend->SetOpacity(0, 0.5);
    //blend->SetOpacity(1, 0.5);
    //blend->Update();

    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(contourTop);
    scalarBar->SetTitle("Dose");

    double t1 = 324.365234375 - 189.1601543;
    double t2 =  -1.0* (508.365234375 - 423.9414063);
    vtkSmartPointer<vtkMatrix4x4> contourToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    static double contourElements[16] = {
        1, 0, 0, t1,
        0, 1, 0, t2,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    contourToRASMatrix->DeepCopy(contourElements);

    // Display the image
    vtkSmartPointer<vtkImageActor> bottomActor = vtkSmartPointer<vtkImageActor>::New();
    bottomActor->SetInputData(mapBottom->GetOutput());
    vtkSmartPointer<vtkImageActor> topActor = vtkSmartPointer<vtkImageActor>::New();
    topActor->SetInputData(colorTop->GetOutput());
    topActor->SetUserMatrix(contourToRASMatrix);

    vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->SetUserMatrix(contourToRASMatrix);
    contourActor->GetProperty()->SetLineWidth(2);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(bottomActor);
    renderer->AddActor(topActor);
    renderer->AddActor(contourActor);
    renderer->AddActor2D(scalarBar);
    
    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);

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
    callback->SetImageReslice(resliceTop, resliceBottom);
    callback->SetInteractor(interactor);

    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

    // Start interaction
    // The Start() method doesn't return until the window is closed by the user
    interactor->Start();
}





