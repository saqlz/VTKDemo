// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "vtkDICOMImageReader.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"
#include "vtkProperty.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkJPEGReader.h"
class vtkWidgetWindowLevelCallback : public vtkCommand
{
public:
    static vtkWidgetWindowLevelCallback *New()
    {
        return new vtkWidgetWindowLevelCallback;
    }

    void Execute(vtkObject *caller, unsigned long vtkNotUsed(event),
        void *callData)
    {
        vtkImagePlaneWidget* self =
            reinterpret_cast< vtkImagePlaneWidget* >(caller);
        if (!self) return;

        double* wl = static_cast<double*>(callData);

        if (self == this->WidgetX)
        {
            this->WidgetY->SetWindowLevel(wl[0], wl[1]);
            this->WidgetZ->SetWindowLevel(wl[0], wl[1]);
        }
        else if (self == this->WidgetY)
        {
            this->WidgetX->SetWindowLevel(wl[0], wl[1]);
            this->WidgetZ->SetWindowLevel(wl[0], wl[1]);
        }
        else if (self == this->WidgetZ)
        {
            this->WidgetX->SetWindowLevel(wl[0], wl[1]);
            this->WidgetY->SetWindowLevel(wl[0], wl[1]);
        }
    }

    vtkWidgetWindowLevelCallback() :WidgetX(0), WidgetY(0), WidgetZ(0) {}

    vtkImagePlaneWidget* WidgetX;
    vtkImagePlaneWidget* WidgetY;
    vtkImagePlaneWidget* WidgetZ;
};

int main()
{
    vtkDICOMImageReader *DicomReader = vtkDICOMImageReader::New();
    DicomReader->SetDataByteOrderToLittleEndian();
    DicomReader->SetDirectoryName("E:\\Images\\Test\\");
    DicomReader->Update();
    vtkOutlineFilter *DicomOutline = vtkOutlineFilter::New();
    DicomOutline->SetInputConnection(DicomReader->GetOutputPort());
    vtkPolyDataMapper *DicomMapper = vtkPolyDataMapper::New();
    DicomMapper->SetInputConnection(DicomOutline->GetOutputPort());
    vtkActor *DicomActor = vtkActor::New();
    DicomActor->SetMapper(DicomMapper);

    vtkRenderWindow *renWin = vtkRenderWindow::New();
    vtkRenderer *ren = vtkRenderer::New();
    vtkRenderer *ren1 = vtkRenderer::New();
    vtkRenderer *ren2 = vtkRenderer::New();
    vtkRenderer *ren3 = vtkRenderer::New();
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    renWin->AddRenderer(ren1);
    renWin->AddRenderer(ren2);
    renWin->AddRenderer(ren3);
    renWin->AddRenderer(ren);
    iren->SetRenderWindow(renWin);
    //        ren->AddActor( DicomActor );

    vtkCellPicker * picker = vtkCellPicker::New();
    picker->SetTolerance(0.005);

    vtkImagePlaneWidget * planeWidgetX = vtkImagePlaneWidget::New();
    planeWidgetX->SetInteractor(iren);
    planeWidgetX->SetKeyPressActivationValue('x');
    planeWidgetX->SetPicker(picker);
    planeWidgetX->RestrictPlaneToVolumeOn();
    planeWidgetX->GetPlaneProperty()->SetColor(0.0, 0.0, 1.0);
    planeWidgetX->DisplayTextOn();
    //planeWidgetX->TextureInterpolateOn();
    planeWidgetX->TextureInterpolateOff();
    planeWidgetX->SetResliceInterpolateToLinear();
    planeWidgetX->SetInputData((vtkDataSet*)DicomReader->GetOutput());
    planeWidgetX->SetPlaneOrientationToXAxes();//??
    planeWidgetX->SetSliceIndex(255);
    planeWidgetX->GetTexturePlaneProperty()->SetOpacity(1);
    planeWidgetX->On();


    vtkImagePlaneWidget * planeWidgetY = vtkImagePlaneWidget::New();
    planeWidgetY->SetInteractor(iren);
    planeWidgetY->SetKeyPressActivationValue('y');
    planeWidgetY->SetPicker(picker);
    planeWidgetY->RestrictPlaneToVolumeOn();
    planeWidgetY->GetPlaneProperty()->SetColor(1.0, 0.0, 0.0);
    planeWidgetY->DisplayTextOn();
    planeWidgetY->TextureInterpolateOn();
    planeWidgetY->SetResliceInterpolateToLinear();
    planeWidgetY->SetInputData((vtkDataSet*)DicomReader->GetOutput());
    planeWidgetY->SetPlaneOrientationToYAxes();//??
    planeWidgetY->SetSliceIndex(255);
    planeWidgetY->On();

    vtkImagePlaneWidget * planeWidgetZ = vtkImagePlaneWidget::New();
    planeWidgetZ->SetInteractor(iren);
    planeWidgetZ->DisplayTextOn();
    planeWidgetZ->RestrictPlaneToVolumeOn();
    planeWidgetZ->SetKeyPressActivationValue('z');
    planeWidgetZ->SetPicker(picker);
    planeWidgetZ->GetPlaneProperty()->SetColor(0.0, 1.0, 0.0);
    planeWidgetZ->TextureInterpolateOn();
    planeWidgetZ->SetResliceInterpolateToLinear();
    planeWidgetZ->SetInputData((vtkDataSet*)DicomReader->GetOutput());
    planeWidgetZ->SetPlaneOrientationToZAxes(); //?á????
    planeWidgetZ->SetSliceIndex(150);

    planeWidgetZ->On();
    /*        vtkImagePlaneWidget *planeWidgetZ = vtkImagePlaneWidget::New();
    planeWidgetZ->SetInteractor( iren );
    planeWidgetZ->SetKeyPressActivationValue( 'z' );
    planeWidgetZ->DisplayTextOn();
    planeWidgetZ->SetPicker( picker );
    planeWidgetZ->GetPlaneProperty()->SetColor( 1.0, 0.0, 0.0 );
    planeWidgetZ->TextureInterpolateOn();
    planeWidgetZ->SetResliceInterpolateToCubic();
    planeWidgetZ->SetInput( (vtkDataSet*)DicomReader->GetOutput() );
    planeWidgetZ->SetPlaneOrientationToZAxes();
    planeWidgetZ->SetSliceIndex( 183 );
    planeWidgetZ->On();*/

    vtkWidgetWindowLevelCallback* cbk = vtkWidgetWindowLevelCallback::New();
    cbk->WidgetX = planeWidgetX;
    cbk->WidgetY = planeWidgetY;
    cbk->WidgetZ = planeWidgetZ;
    cbk->Delete();

    vtkImageMapToColors *colorMap1 = vtkImageMapToColors::New();
    //colorMap1->PassAlphaToOutputOff(); //use in RGBA
    colorMap1->SetActiveComponent(0);
    colorMap1->SetOutputFormatToLuminance();
    colorMap1->SetInputData((vtkDataSet*)planeWidgetX->GetResliceOutput());
    colorMap1->SetLookupTable((vtkScalarsToColors *)planeWidgetX->GetLookupTable());
    vtkImageActor * imageActor1 = vtkImageActor::New();
    imageActor1->PickableOff();
    imageActor1->SetInputData(colorMap1->GetOutput());

    vtkImageMapToColors *colorMap2 = vtkImageMapToColors::New();
    colorMap2->PassAlphaToOutputOff();
    colorMap2->SetActiveComponent(0); // for muti-component
    colorMap2->SetOutputFormatToLuminance();
    colorMap2->SetInputData((vtkDataSet*)planeWidgetY->GetResliceOutput());
    colorMap2->SetLookupTable((vtkScalarsToColors *)planeWidgetX->GetLookupTable());
    vtkImageActor * imageActor2 = vtkImageActor::New();
    imageActor2->PickableOff();
    imageActor2->SetInputData(colorMap2->GetOutput());

    vtkImageMapToColors *colorMap3 = vtkImageMapToColors::New();
    colorMap3->PassAlphaToOutputOff();
    colorMap3->SetActiveComponent(0);
    colorMap3->SetOutputFormatToLuminance();
    colorMap3->SetInputData((vtkDataSet*)planeWidgetZ->GetResliceOutput());
    colorMap3->SetLookupTable((vtkScalarsToColors *)planeWidgetX->GetLookupTable());
    //colorMap3->SetLookupTable(planeWidgetX->GetLookupTable());
    vtkImageActor *imageActor3 = vtkImageActor::New();
    imageActor3->PickableOff();
    imageActor3->SetInputData(colorMap3->GetOutput());

    ren->AddActor(DicomActor); //outline
    ren1->AddActor(imageActor1);
    ren2->AddActor(imageActor2);
    ren3->AddActor(imageActor3);

    // OK
    ren->SetBackground(0.3, 0.3, 0.6);
    ren1->SetBackground(1.0, 0.0, 0.0);
    ren2->SetBackground(0.0, 1.0, 0.0);
    ren3->SetBackground(0.0, 0.0, 1.0);
    renWin->SetSize(600, 400);
    ren->SetViewport(0, 0.5, 0.5, 1);
    ren1->SetViewport(0.5, 0.5, 1, 1);
    ren2->SetViewport(0, 0, 0.5, 0.5);
    ren3->SetViewport(0.5, 0, 1, 0.5);



    iren->Initialize();
    iren->Start();
    renWin->Render();

    return 0;
}