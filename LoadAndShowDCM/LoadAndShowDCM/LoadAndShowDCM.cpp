// LoadAndShowDCM.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

int main()
{
    std::string sPath = "E:\\Images\\Test\\";
    int iWidth = 640;
    int iHeight = 480;

    //Step0. ��Load DICOM Data
    //Step0. ����DIOCMԭʼ����
    vtkSmartPointer<vtkDICOMImageReader> v16 = vtkSmartPointer<vtkDICOMImageReader>::New();
    v16->SetDataByteOrderToLittleEndian();
    v16->SetDirectoryName(sPath.c_str());
    v16->Update();

    vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper =
        vtkSmartPointer<vtkVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(v16->GetOutput());

    //���ù��߲�������  
    //volumeMapper->SetSampleDistance(volumeMapper->GetSampleDistance()*4);  
    //����ͼ���������  
    //volumeMapper->SetAutoAdjustSampleDistances(0);  
    //volumeMapper->SetImageSampleDistance(4);  
    /*************************************************************************/
    vtkSmartPointer<vtkVolumeProperty> volumeProperty =
        vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();  //�򿪻��߹ر���Ӱ����  
    volumeProperty->SetAmbient(0.4);
    volumeProperty->SetDiffuse(0.6);  //������  
    volumeProperty->SetSpecular(0.2); //���淴��  
                                      //���ò�͸����  
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
        vtkSmartPointer<vtkPiecewiseFunction>::New();
    compositeOpacity->AddPoint(70, 0.00);
    compositeOpacity->AddPoint(90, 0.40);
    compositeOpacity->AddPoint(180, 0.60);
    volumeProperty->SetScalarOpacity(compositeOpacity); //���ò�͸���ȴ��亯��  
                                                        //compositeOpacity->AddPoint(120,  0.00);//�������ز�������,�ԱȲ�ͬ������  
                                                        //compositeOpacity->AddPoint(180,  0.60);  
                                                        //volumeProperty->SetScalarOpacity(compositeOpacity);  
                                                        //�����ݶȲ�͸������  
    vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity =
        vtkSmartPointer<vtkPiecewiseFunction>::New();
    volumeGradientOpacity->AddPoint(10, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.5);
    volumeGradientOpacity->AddPoint(100, 1.0);
    volumeProperty->SetGradientOpacity(volumeGradientOpacity);//�����ݶȲ�͸����Ч���Ա�  
                                                              //������ɫ����  
    vtkSmartPointer<vtkColorTransferFunction> color =
        vtkSmartPointer<vtkColorTransferFunction>::New();
    color->AddRGBPoint(0.000, 0.00, 0.00, 0.00);
    color->AddRGBPoint(64.00, 1.00, 0.52, 0.30);
    color->AddRGBPoint(190.0, 1.00, 1.00, 1.00);
    color->AddRGBPoint(220.0, 0.20, 0.20, 0.20);
    volumeProperty->SetColor(color);
    /********************************************************************************/
    vtkSmartPointer<vtkVolume> volume =
        vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren->SetBackground(0, 0, 0);
    ren->AddVolume(volume);

    vtkSmartPointer<vtkRenderWindow> rw = vtkSmartPointer<vtkRenderWindow>::New();
    rw->AddRenderer(ren);
    rw->SetSize(640, 480);
    rw->Render();
    rw->SetWindowName("VolumeRendering PipeLine");

    vtkSmartPointer<vtkRenderWindowInteractor> rwi =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    rwi->SetRenderWindow(rw);
    /********************************************************************************/
    //vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();  
    //axes->SetScale(10);  
    //vtkSmartPointer<vtkOrientationMarkerWidget> widget =  
    //  vtkSmartPointer<vtkOrientationMarkerWidget>::New();  
    //widget->SetOutlineColor(1, 1, 1);  
    //widget->SetViewport(0, 0, 0.2, 0.2);  
    //widget->SetOrientationMarker(axes);  
    //widget->SetInteractor(rwi);  
    //widget->SetEnabled(1);  
    //widget->InteractiveOn();  

    ren->ResetCamera();
    rw->Render();
    rwi->Start();
    return 0;
}

