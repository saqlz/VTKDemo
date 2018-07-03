// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h> 
#include <string.h> 

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
#include <vector>
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPlanarContourToClosedSurfaceConversionRule.h"
#include "vtkPlanarContourToRibbonModelConversionRule.h"


void TestBlendCTImageAndDose();
void TestLoadContour();

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
        Slicing = 0;
        ImageReslice = 0;
        ComImageReslice = 0;
        Interactor = 0;
    };

    void SetImageReslice(vtkImageReslice *reslice1, vtkImageReslice *reslice2) {
        ImageReslice = reslice1;
        ComImageReslice = reslice2;
    };

    vtkImageReslice *GetImageReslice() {
        return ImageReslice;
    };

    void SetInteractor(vtkRenderWindowInteractor *interactor) {
        Interactor = interactor;
    };

    vtkRenderWindowInteractor *GetInteractor() {
        return Interactor;
    };

    void Execute(vtkObject *, unsigned long event, void *) VTK_OVERRIDE
    {
        vtkRenderWindowInteractor *interactor = GetInteractor();

        int lastPos[2];
        interactor->GetLastEventPosition(lastPos);
        int currPos[2];
        interactor->GetEventPosition(currPos);

        if (event == vtkCommand::LeftButtonPressEvent)
        {
            Slicing = 1;
        }
        else if (event == vtkCommand::LeftButtonReleaseEvent)
        {
            Slicing = 0;
        }
        else if (event == vtkCommand::MouseMoveEvent)
        {
            if (Slicing)
            {
                vtkImageReslice *reslice = ImageReslice;

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

                ComImageReslice->Update();
                double center2[4];
                vtkMatrix4x4 *matrix2 = ComImageReslice->GetResliceAxes();
                matrix2->MultiplyPoint(point, center2);
                matrix2->SetElement(0, 3, center2[0]);
                matrix2->SetElement(1, 3, center2[1]);
                matrix2->SetElement(2, 3, center2[2]);
               
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

typedef itk::Image<unsigned char, 2>  UnsignedCharImageType;
typedef itk::Image<float, 2>  FloatImageType;

static void CreateImage(UnsignedCharImageType::Pointer image);

int main(int, char *[])
{
    //vtkSmartPointer<vtkRenderWindow> renWin =
    //    vtkSmartPointer<vtkRenderWindow>::New();

    //std::string sPath = "E:\\Images\\Test\\";
    //vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    //reader->SetDataByteOrderToLittleEndian();
    //reader->SetDirectoryName(sPath.c_str());
    //reader->Update();

    ////vtkNew<vtkImageHistogram> histogram;
    ////histogram->AutomaticBinningOff();
    ////histogram->SetNumberOfBins(200);
    ////histogram->SetBinOrigin(0);
    ////histogram->SetBinSpacing(1);
    ////
    ////histogram->SetInputConnection(reader->GetOutputPort());
    ////histogram->GenerateHistogramImageOn();
    ////histogram->SetHistogramImageSize(256, 256);
    ////histogram->SetHistogramImageScaleToLog();
    //////histogram->AutomaticBinningOn();
    ////histogram->Update();

    ////vtkIdType nbins = histogram->GetNumberOfBins();
    ////double range[2];
    ////range[0] = histogram->GetBinOrigin();
    ////range[1] = range[0] + (nbins - 1)*histogram->GetBinSpacing();

    ////for (int i = 0; i < 2; i++)
    ////{
    ////    vtkSmartPointer<vtkRenderer> renderer =
    ////        vtkSmartPointer<vtkRenderer>::New();
    ////    vtkCamera *camera = renderer->GetActiveCamera();
    ////    renderer->SetBackground(0.0, 0.0, 0.0);
    ////    renderer->SetViewport(0.5*(i & 1), 0.0,
    ////        0.5 + 0.5*(i & 1), 1.0);
    ////    renWin->AddRenderer(renderer);

    ////    vtkSmartPointer<vtkImageSliceMapper> imageMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    ////    if ((i & 1) == 0)
    ////    {
    ////        imageMapper->SetInputConnection(reader->GetOutputPort());
    ////    }
    ////    else
    ////    {
    ////        imageMapper->SetInputConnection(histogram->GetOutputPort());
    ////        imageMapper->BorderOn();
    ////    }

    ////    const double *bounds = imageMapper->GetBounds();
    ////    double point[3];
    ////    point[0] = 0.5*(bounds[0] + bounds[1]);
    ////    point[1] = 0.5*(bounds[2] + bounds[3]);
    ////    point[2] = 0.5*(bounds[4] + bounds[5]);

    ////    camera->SetFocalPoint(point);
    ////    point[imageMapper->GetOrientation()] += 500.0;
    ////    camera->SetPosition(point);
    ////    camera->SetViewUp(0.0, 1.0, 0.0);
    ////    camera->ParallelProjectionOn();
    ////    camera->SetParallelScale(128);

    ////    vtkSmartPointer<vtkImageSlice> image = vtkSmartPointer<vtkImageSlice>::New();
    ////    image->SetMapper(imageMapper);

    ////    renderer->AddViewProp(image);

    ////    if ((i & 1) == 0)
    ////    {
    ////        image->GetProperty()->SetColorWindow(range[1] - range[0]);
    ////        image->GetProperty()->SetColorLevel(0.5*(range[0] + range[1]));
    ////    }
    ////    else
    ////    {
    ////        image->GetProperty()->SetInterpolationTypeToNearest();
    ////        image->GetProperty()->SetColorWindow(255.0);
    ////        image->GetProperty()->SetColorLevel(127.5);
    ////    }
    ////}

    //int bins = 300;
    //int comps = 1;

    //vtkImageAccumulate *histogram = vtkImageAccumulate::New();
    //histogram->SetInputConnection(reader->GetOutputPort());
    //histogram->SetComponentExtent(0, bins - 1, 0, 0, 0, 0);
    //histogram->SetComponentOrigin(0, 0, 0);
    //histogram->SetComponentSpacing(histogram->/ bins, 0, 0);
    //histogram->Update();

    //int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());
    //vtkIntArray* frequencies = vtkIntArray::New();
    //frequencies->SetNumberOfComponents(1);
    //for (int j = 0; j < bins; ++j)
    //{
    //    for (int i = 0; i < comps; i++)
    //    {
    //        frequencies->InsertNextTuple1(*output++);
    //    }
    //}

    //vtkDataObject *dataObject = vtkDataObject::New();
    //dataObject->GetFieldData()->AddArray(frequencies);

    //vtkSmartPointer<vtkBarChartActor> barChart = vtkSmartPointer<vtkBarChartActor>::New();
    //barChart->SetInput(dataObject);
    //barChart->SetTitle("Histogram");
    //barChart->GetPositionCoordinate()->SetValue(0.05, 0.05, 0.0);
    //barChart->GetPosition2Coordinate()->SetValue(0.95, 0.95, 0.0);
    //barChart->GetProperty()->SetColor(0, 0, 0);
    //barChart->GetTitleTextProperty()->SetColor(0, 0, 0);
    //barChart->GetLabelTextProperty()->SetColor(0, 0, 0);
    //barChart->GetLegendActor()->SetNumberOfEntries(dataObject->GetFieldData()->GetArray(0)->GetNumberOfTuples());
    //barChart->LegendVisibilityOff();
    //barChart->LabelVisibilityOff();

    //vtkSmartPointer<vtkRenderer> renderer =
    //    vtkSmartPointer<vtkRenderer>::New();
    //renderer->SetBackground(1.0, 1.0, 1.0);
    //renderer->AddActor(barChart);
    //renWin->AddRenderer(renderer);
    //vtkSmartPointer<vtkInteractorStyleImage> imageStyle =
    //    vtkSmartPointer<vtkInteractorStyleImage>::New();
    //vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    //    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //interactor->SetInteractorStyle(imageStyle);
    //renWin->SetInteractor(interactor);
    //renWin->SetSize(512, 256);
    //renWin->Render();
    //interactor->Start();


std::map<int, double> mapThickness;
for (int i = 0; i < 80; ++i)
{
    mapThickness.insert(std::make_pair(i, 2));
}
const double length = 100;
const double X1Position = -40;
const double Y1Position = -40;
const double X2Position = 40;
const double Y2Position = 40;

//Below is update Code
vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
auto leaveNumberOfMLC = mapThickness.size();


vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
//vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
//polyLine->GetPointIds()->SetNumberOfIds(4);
//polyLine->GetPointIds()->SetId(0, 0);
//polyLine->GetPointIds()->SetId(1, 1);
//cells->InsertNextCell(polyLine);
for (int i = 0; i < leaveNumberOfMLC; ++i)
{
    points->InsertNextPoint(-1.0 * length, -1.0 * length + i * mapThickness[i], 0);
    points->InsertNextPoint(length, -1.0 * length + i * mapThickness[i], 0);
    points->InsertNextPoint(length, -1.0 * length + (i+ 1)* mapThickness[i] - 0.1, 0);
    points->InsertNextPoint(-1.0 * length, -1.0 * length + (i + 1)* mapThickness[i] - 0.1, 0);
    
    vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(4);
    polyLine->GetPointIds()->SetId(0, 4 * i + 0);
    polyLine->GetPointIds()->SetId(1, 4 * i + 1);
    polyLine->GetPointIds()->SetId(2, 4 * i + 2);
    polyLine->GetPointIds()->SetId(3, 4 * i + 3);
    cells->InsertNextCell(polyLine);
}

// Create a polydata to store everything in
vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

// Add the points to the dataset
polyData->SetPoints(points);


// Add the lines to the dataset
polyData->SetPolys(cells);



// Setup actor and mapper
vtkSmartPointer<vtkPolyDataMapper> mapper =
vtkSmartPointer<vtkPolyDataMapper>::New();
mapper->SetInputData(polyData);
vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
actor->SetMapper(mapper);
actor->GetProperty()->SetColor(62.0 / 255.0, 80.0 / 255.0, 100.0 / 255.0);
//-------------------------//
vtkSmartPointer<vtkPoints> jawPoints = vtkSmartPointer<vtkPoints>::New();
jawPoints->InsertNextPoint(X1Position, Y1Position, 0);
jawPoints->InsertNextPoint(X2Position, Y1Position, 0);
jawPoints->InsertNextPoint(X2Position, Y2Position, 0);
jawPoints->InsertNextPoint(X1Position, Y2Position, 0);

vtkSmartPointer<vtkPolyLine> jawPolyLine = vtkSmartPointer<vtkPolyLine>::New();
jawPolyLine->GetPointIds()->SetNumberOfIds(5);
jawPolyLine->GetPointIds()->SetId(0, 0);
jawPolyLine->GetPointIds()->SetId(1, 1);
jawPolyLine->GetPointIds()->SetId(2, 2);
jawPolyLine->GetPointIds()->SetId(3, 3);
jawPolyLine->GetPointIds()->SetId(4, 0);

vtkSmartPointer<vtkCellArray> jawCells = vtkSmartPointer<vtkCellArray>::New();
jawCells->InsertNextCell(jawPolyLine);

// Create a polydata to store everything in
vtkSmartPointer<vtkPolyData> jawPolyData = vtkSmartPointer<vtkPolyData>::New();

// Add the points to the dataset
jawPolyData->SetPoints(jawPoints);

// Add the lines to the dataset
jawPolyData->SetLines(jawCells);
vtkSmartPointer<vtkPolyDataMapper> jawMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
jawMapper->SetInputData(jawPolyData);
vtkSmartPointer<vtkActor> jawActor = vtkSmartPointer<vtkActor>::New();
jawActor->SetMapper(jawMapper);
jawActor->GetProperty()->SetColor(18.0 / 255.0, 20.0 / 255.0, 19.0 / 255.0);
jawActor->GetProperty()->SetLineWidth(2);

// Setup render window, renderer, and interactor
vtkSmartPointer<vtkRenderer> renderer =
vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkRenderWindow> renderWindow =
vtkSmartPointer<vtkRenderWindow>::New();
renderWindow->AddRenderer(renderer);
vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
vtkSmartPointer<vtkRenderWindowInteractor>::New();
renderWindowInteractor->SetRenderWindow(renderWindow);
// Setup the text and add it to the renderer  
vtkTextProperty*pTexProper = vtkTextProperty::New(); //声明文本属性
pTexProper->SetColor(1, 1, 0);
pTexProper->SetFontSize(18);
pTexProper->SetFontFamily(0);
pTexProper->SetJustification(1);
pTexProper->SetBold(1);
pTexProper->SetItalic(1);
pTexProper->SetShadow(1);
vtkSmartPointer textActor = vtkSmartPointer::New(); //声明3D文本
textActor->SetInput("hengshui");
textActor->SetTextProperty(pTexProper);
textActor->SetPosition(80, 80, 5); //设置位置
textActor->SetScale(0.7, 0.7, 0.7); //设置文字大小
textActor->RotateX(90.0)


vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
textActor->SetInput ("1111");
textActor->SetPosition(X2Position, Y2Position);
textActor->GetTextProperty()->SetFontSize (5);
textActor->GetTextProperty()->SetColor ( 1.0, 0.0, 0.0 );
renderer->AddActor2D( textActor );
renderer->AddActor(actor);
renderer->AddActor(jawActor);
renderer->SetBackground(19.0 / 255.0, 49.0 / 255.0, 76.0 / 255.0);
renderWindow->Render();
renderWindowInteractor->Start();



    return EXIT_SUCCESS;
}

void CreateImage(UnsignedCharImageType::Pointer image)
{
    // Create an image
    itk::Index<2> start;
    start.Fill(0);

    itk::Size<2> size;
    size.Fill(100);

    itk::ImageRegion<2> region(start, size);
    image->SetRegions(region);
    image->Allocate();
    image->FillBuffer(0);

    // Create a line of white pixels
    for (unsigned int i = 40; i < 60; ++i)
    {
        itk::Index<2> pixel;
        pixel.Fill(i);
        image->SetPixel(pixel, 255);
    }

    // Create another line of pixels
    for (unsigned int i = 10; i < 20; ++i)
    {
        itk::Index<2> pixel;
        pixel[0] = 10;
        pixel[1] = i;
        image->SetPixel(pixel, 255);
    }

    typedef  itk::ImageFileWriter< UnsignedCharImageType  > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("image.png");
    writer->SetInput(image);
    writer->Update();
}

void TestBlendCTImageAndDose()
{
    // Matrices for axial, coronal, sagittal, oblique view orientations
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    //Bottom Image
    std::string sBottomImagePath = "D:\\GitHub\\DeepPlan\\TestCT.raw";
    int iBottomImageVolumeDimension[3] = { 512, 512, 53 };
    int iBottomImageVolumeComponent = 1;
    //double dBottomOrigin[3] = { -233.0439453125, -327.0439453125, 410 };
    double dBottomOrigin[3] = {0, 0, 0};
    double dBottomSpacing[3] = { 0.912109375, 0.912109375, 5 };
    vtkSmartPointer<vtkImageData> bottomImageData = vtkSmartPointer<vtkImageData>::New();
    bottomImageData->SetDimensions(iBottomImageVolumeDimension);
    bottomImageData->AllocateScalars(VTK_INT, iBottomImageVolumeComponent);
    bottomImageData->SetOrigin(dBottomOrigin);
    bottomImageData->SetSpacing(dBottomSpacing);
    int* bottomImageVolumeData = LoadRawVolume<int>(iBottomImageVolumeDimension, sBottomImagePath, iBottomImageVolumeComponent);
    int* bottomImagePtr = reinterpret_cast<int*>(bottomImageData->GetScalarPointer());
    for (int i = 0; i < iBottomImageVolumeDimension[0] * iBottomImageVolumeDimension[1] * iBottomImageVolumeDimension[2] * iBottomImageVolumeComponent; i += iBottomImageVolumeComponent)
    {
        bottomImagePtr[i] = bottomImageVolumeData[i];
    }

    //Top Image
    std::string sTopImagePath = "D:\\GitHub\\DeepPlan\\TestMR.raw";
    int iTopImageVolumeDimension[3] = { 512, 512, 248 };
    int iTopImageVolumeComponent = 1;
    //double dTopOrigin[3] = { -113.619, -150.906, -110.923 };
    double dTopOrigin[3] = {0, 0, 0};
    double dTopSpacing[3] = { 0.4688, 0.4688, 0.9 };
    vtkSmartPointer<vtkImageData> topImageData = vtkSmartPointer<vtkImageData>::New();
    topImageData->SetDimensions(iTopImageVolumeDimension);
    topImageData->AllocateScalars(VTK_INT, iTopImageVolumeComponent);
    topImageData->SetOrigin(dTopOrigin);
    topImageData->SetSpacing(dTopSpacing);
    int* topImageVolumeData = LoadRawVolume<int>(iTopImageVolumeDimension, sTopImagePath, iTopImageVolumeComponent);
    int* topImagePtr = reinterpret_cast<int*>(topImageData->GetScalarPointer());
    for (int i = 0; i < iTopImageVolumeDimension[0] * iTopImageVolumeDimension[1] * iTopImageVolumeDimension[2] * iTopImageVolumeComponent; i += iTopImageVolumeComponent)
    {
        topImagePtr[i] = topImageVolumeData[i];
    }

    double centerBottom[3];
    centerBottom[0] = dBottomOrigin[0];
    centerBottom[1] = dBottomOrigin[1];
    centerBottom[2] = dBottomOrigin[2]; // + dBottomSpacing[2] * 0.5 * iBottomImageVolumeDimension[2];
    
    vtkSmartPointer<vtkMatrix4x4> resliceMatrixBttom = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceMatrixBttom->DeepCopy(axialElements);
    resliceMatrixBttom->SetElement(0, 3, centerBottom[0]);
    resliceMatrixBttom->SetElement(1, 3, centerBottom[1]);
    resliceMatrixBttom->SetElement(2, 3, centerBottom[2]);
    vtkSmartPointer<vtkImageReslice> resliceBottom = vtkSmartPointer<vtkImageReslice>::New();
    resliceBottom->SetInputData(bottomImageData);
    resliceBottom->SetOutputDimensionality(2);
    resliceBottom->SetResliceAxes(resliceMatrixBttom);
    resliceBottom->SetInterpolationModeToLinear();
    resliceBottom->Update();

    vtkSmartPointer<vtkLookupTable> tableBottom = vtkSmartPointer<vtkLookupTable>::New();
    tableBottom->SetRange(-300, 350);          // image intensity range
    tableBottom->SetValueRange(0.0, 1.0);      // from black to white
    tableBottom->SetSaturationRange(0.0, 0.0); // no color saturation
    tableBottom->SetRampToLinear();
    tableBottom->Build();
    vtkSmartPointer<vtkImageMapToColors> colorBottom = vtkSmartPointer<vtkImageMapToColors>::New();
    colorBottom->SetLookupTable(tableBottom);
    colorBottom->SetInputConnection(resliceBottom->GetOutputPort());
    vtkSmartPointer<vtkImageActor> actorBottom = vtkSmartPointer<vtkImageActor>::New();
    actorBottom->GetMapper()->SetInputConnection(colorBottom->GetOutputPort());
    
    vtkSmartPointer<vtkMatrix4x4> resliceMatrixTop = vtkSmartPointer<vtkMatrix4x4>::New();
    static double axialElementsTop[16] = {
        0.994037, 0.106591, -0.0230048, 0,
        -0.108939, 0.961397, -0.252682, 0,
        -0.00481705, 0.253682, 0.967276, -100.7,
         0, 0, 0, 1 };

    resliceMatrixTop->DeepCopy(axialElementsTop);
    double centerTop[4];
    double pointTop[4];
    pointTop[0] = centerBottom[0];
    pointTop[1] = centerBottom[1];
    pointTop[2] = centerBottom[2];
    pointTop[3] = 1.0;
    resliceMatrixTop->MultiplyPoint(pointTop, centerTop);
    resliceMatrixTop->SetElement(0, 3, centerTop[0]);
    resliceMatrixTop->SetElement(1, 3, centerTop[1]);
    resliceMatrixTop->SetElement(2, 3, centerTop[2]);
    vtkSmartPointer<vtkImageReslice> resliceTop = vtkSmartPointer<vtkImageReslice>::New();
    resliceTop->SetInputData(topImageData);
    resliceTop->SetOutputDimensionality(2);
    resliceTop->SetResliceAxes(resliceMatrixTop);
    resliceTop->SetInterpolationModeToLinear();
    resliceTop->Update();

    vtkSmartPointer<vtkLookupTable> tableTop = vtkSmartPointer<vtkLookupTable>::New();
    tableTop->SetTableRange(0, 12000);
    tableTop->SetValueRange(0.3, 0.8);
    tableTop->SetSaturationRange(0.0, 1.0);
    tableTop->SetHueRange(0.12, 0.12);
    tableTop->SetAlphaRange(0.5, 1);
    tableTop->Build();
    vtkSmartPointer<vtkImageMapToColors> colorTop = vtkSmartPointer<vtkImageMapToColors>::New();
    colorTop->SetLookupTable(tableTop);
    colorTop->SetInputConnection(resliceTop->GetOutputPort());
    vtkSmartPointer<vtkImageActor> actorTop = vtkSmartPointer<vtkImageActor>::New();
    actorTop->GetMapper()->SetInputConnection(colorTop->GetOutputPort());

    vtkSmartPointer<vtkMatrix4x4> contourToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    static double contourElements[16] = {
        1, 0, 0, 113.4872,
        0, 1, 0, 70,
        0, 0, 1, 0,
        0, 0, 0, 1 };
    contourToRASMatrix->DeepCopy(contourElements);
    actorTop->SetUserMatrix(contourToRASMatrix);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actorBottom);
    renderer->AddActor(actorTop);

    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
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
    callback->SetImageReslice(resliceBottom, resliceTop);
    callback->SetInteractor(interactor);

    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

    // Start interaction
    // The Start() method doesn't return until the window is closed by the user
    interactor->Start();
}

void TestLoadContour() 
{
    // Create containers for contour poly data
    vtkSmartPointer<vtkPoints> currentRoiContourPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> currentRoiContourCells = vtkSmartPointer<vtkCellArray>::New();
    
    std::string sBottomImagePath = "D:\\GitHub\\WisdomRay\\WisdomRay\\appdata\\ContourData.txt";
    std::ifstream infile(sBottomImagePath, std::ios::in);
    std::string lineStr;
    vtkIdType pointId = 0;
    std::vector<float> vecPoints;
    while (std::getline(infile, lineStr))
    {
        if (lineStr.find("Contour") != std::string::npos)
        {
            if (!vecPoints.empty()) 
            {
                int numberOfPoints = (int)(vecPoints.size() / 3);
                int contourIndex = currentRoiContourCells->InsertNextCell(numberOfPoints + 1);
                for (size_t i = 0; i < vecPoints.size(); i += 3)
                {
                    currentRoiContourPoints->InsertPoint(pointId, vecPoints[i], vecPoints[i+1], vecPoints[i+2]);
                    currentRoiContourCells->InsertCellPoint(pointId);
                    pointId++;
                }
                currentRoiContourCells->InsertCellPoint(pointId - numberOfPoints);
            }
            vecPoints.clear();
            continue;
        }
        float px, py, pz;
        sscanf(lineStr.c_str(), "%f,%f,%f", &px, &py, &pz);
        vecPoints.push_back(px);
        vecPoints.push_back(py);
        vecPoints.push_back(pz);
    }

    vtkSmartPointer<vtkPolyData> currentRoiPolyData = vtkSmartPointer<vtkPolyData>::New();
    currentRoiPolyData->SetPoints(currentRoiContourPoints);
    currentRoiPolyData->SetLines(currentRoiContourCells);

    static double contourElements[16] = {
        0, 1, 0, 0,
        0, 0, 1, 0,
        1, 0, 0, 0,
        0, 0, 0, 1 };
    vtkSmartPointer<vtkMatrix4x4> contourToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    contourToRASMatrix->DeepCopy(contourElements);
    vtkSmartPointer<vtkTransform> WorldToSliceTransform = vtkSmartPointer<vtkTransform>::New();
    WorldToSliceTransform->SetMatrix(contourToRASMatrix);

    vtkNew<vtkActor2D> PolyDataFillActor;
    vtkNew<vtkActor2D> PolyDataOutlineActor;
    vtkNew<vtkActor2D> ImageOutlineActor;
    vtkSmartPointer<vtkCutter> Cutter;
    vtkNew<vtkTransformPolyDataFilter> ModelWarper;
    vtkSmartPointer<vtkPlane> Plane;
    vtkNew<vtkStripper> Stripper;
    vtkNew<vtkCleanPolyData> Cleaner;
    vtkNew<vtkTriangleFilter> TriangleFilter;
   
    vtkSmartPointer<vtkGeneralTransform> NodeToWorldTransform;
    vtkSmartPointer<vtkGeneralTransform> WorldToNodeTransform;

    Cutter = vtkSmartPointer<vtkCutter>::New();
    Plane = vtkSmartPointer<vtkPlane>::New();
    Cutter->SetInputConnection(ModelWarper->GetOutputPort());
    Cutter->SetCutFunction(Plane);
    Cutter->SetGenerateCutScalars(0);
    
    vtkSmartPointer<vtkTransformPolyDataFilter> polyDataOutlineTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    polyDataOutlineTransformer->SetInputConnection(Cutter->GetOutputPort());
    polyDataOutlineTransformer->SetTransform(WorldToSliceTransform);
    vtkSmartPointer<vtkPolyDataMapper2D> polyDataOutlineMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    polyDataOutlineMapper->SetInputConnection(polyDataOutlineTransformer->GetOutputPort());
    PolyDataOutlineActor->SetMapper(polyDataOutlineMapper);
    PolyDataOutlineActor->SetVisibility(0);

    Stripper->SetInputConnection(Cutter->GetOutputPort());
    Cleaner->SetInputConnection(NULL); // This will be modified in the UpdateDisplayNodePipeline function
    TriangleFilter->SetInputConnection(Cleaner->GetOutputPort());
    vtkSmartPointer<vtkTransformPolyDataFilter> polyDataFillTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    polyDataFillTransformer->SetInputConnection(TriangleFilter->GetOutputPort());
    polyDataFillTransformer->SetTransform(WorldToSliceTransform);
    vtkSmartPointer<vtkPolyDataMapper2D> polyDataFillMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    polyDataFillMapper->SetInputConnection(polyDataFillTransformer->GetOutputPort());
    PolyDataFillActor->SetMapper(polyDataFillMapper);
    PolyDataFillActor->SetVisibility(0);

    ModelWarper->SetInputData(currentRoiPolyData);
    ModelWarper->SetTransform(WorldToSliceTransform);
    
    double normal[3] = { 1.2695312500000000,0.0,0.0 };
    double origin[3] = { 0.0,0.0,0.0 };
    Plane->SetNormal(normal);
    Plane->SetOrigin(origin);
    Stripper->SetMaximumLength(10000);
    Stripper->Update();
    vtkCellArray* strippedLines = Stripper->GetOutput()->GetLines();
    vtkSmartPointer<vtkCellArray> closedCells = vtkSmartPointer<vtkCellArray>::New();
    strippedLines->InitTraversal();
    vtkSmartPointer<vtkIdList> pointList = vtkSmartPointer<vtkIdList>::New();
    while (strippedLines->GetNextCell(pointList))
    {
        if (pointList->GetNumberOfIds() > 0
            && pointList->GetId(0) == pointList->GetId(pointList->GetNumberOfIds() - 1))
        {
            closedCells->InsertNextCell(pointList);
        }
    }

    std::cout << closedCells << std::endl;













    //vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    //    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //renderWindowInteractor->SetRenderWindow(renderWindow);

    //renderWindow->Render();
    //renderWindowInteractor->Start();
}
