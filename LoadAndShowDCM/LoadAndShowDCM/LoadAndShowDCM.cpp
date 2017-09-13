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
  
    //ѡ��������㷨
    vtkSmartPointer<vtkOpenGLProjectedTetrahedraMapper> volumeMapper = vtkSmartPointer<vtkOpenGLProjectedTetrahedraMapper>::New();
    volumeMapper->SetInputData(v16->GetOutput());
    //���ù��߲�������  
    //volumeMapper->SetSampleDistance(volumeMapper->GetSampleDistance()*4);  
    //����ͼ���������  
    //volumeMapper->SetAutoAdjustSampleDistances(0);  
    //volumeMapper->SetImageSampleDistance(4);  
    /*************************************************************************/

    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();  //�򿪻��߹ر���Ӱ����  
    volumeProperty->SetAmbient(0.1);   //������
    volumeProperty->SetDiffuse(0.1);   //������
    volumeProperty->SetSpecular(0.5);  //���淴��
   
    //���ò�͸����  
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    compositeOpacity->AddPoint(70, 0.00);
    compositeOpacity->AddPoint(90, 0.80);
    compositeOpacity->AddPoint(180, 0.90);
    volumeProperty->SetScalarOpacity(compositeOpacity); //���ò�͸���ȴ��亯��  
    //compositeOpacity->AddPoint(120,  0.00);//�������ز�������,�ԱȲ�ͬ������  
    //compositeOpacity->AddPoint(180,  0.60);  
    //volumeProperty->SetScalarOpacity(compositeOpacity);  

    //�����ݶȲ�͸������  
    vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    volumeGradientOpacity->AddPoint(10, 0.0);
    volumeGradientOpacity->AddPoint(90, 0.2);
    volumeGradientOpacity->AddPoint(100, 1.0);
    volumeProperty->SetGradientOpacity(volumeGradientOpacity);//�����ݶȲ�͸����Ч���Ա�  

    //������ɫ����  
    vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
    color->AddRGBPoint(0.000, 0.00, 0.00, 0.00);
    color->AddRGBPoint(64.00, 1.00, 0.52, 0.30);
    color->AddRGBPoint(128, 1.00, 1.00, 1.00);
    color->AddRGBPoint(256.0, 0.20, 0.20, 0.20);
    volumeProperty->SetColor(color);

    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    //������Ⱦ�Ĵ��ڣ�ָ��ΪvtkWin32OpenGLRenderWindow����vtkRenderer���������
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
    renderWindow->SetSize(iWidth, iHeight);              //���ô�С
    //renderWindow->OffScreenRenderingOn();                //������Ⱦ������ʾ����

    //����һ����Ⱦ����Ұ,һ���ΪViewPort����Cell
    //���ü�¼ͼ����ά��Ϣת�����ۺ�����Ԫ�أ����������
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);                //ע��һ��vtkRenderWindowֻ�ܾۺ�һ��vtkRenderer����

    //�����������λ��λ��
    vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();
    aCamera->SetViewUp(0, 0, -1);
    aCamera->SetPosition(0, 1, 0);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);

    renderer->SetBackground(0, 0, 0);                   //���ñ���Ϊ��ɫ
    renderer->AddVolume(volume);                        //����Դ
    renderer->SetActiveCamera(aCamera);                 //���
    renderer->ResetCamera();
    renderWindow->Render();                             //��Ⱦ����ʱ�Ѿ���Ⱦ������
    vtkSmartPointer<vtkRenderWindowInteractor> rwi =  vtkSmartPointer<vtkRenderWindowInteractor>::New();
    rwi->SetRenderWindow(renderWindow);
    rwi->Start();
    return 0;
}

