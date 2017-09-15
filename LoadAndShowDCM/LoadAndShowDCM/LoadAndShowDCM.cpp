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
    vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
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
    volumeProperty->SetAmbient(0.4);   //������
    volumeProperty->SetDiffuse(0.6);   //������
    volumeProperty->SetSpecular(0.5);  //���淴��
   
    //���ò�͸������WWWL����һ�� 
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(55, 0.0);
    opacityTransferFunction->AddPoint(140, 164.721 / 255);
    opacityTransferFunction->AddPoint(305.417, 231.458 / 255);
    opacityTransferFunction->AddPoint(602, 235.0 / 255);
    opacityTransferFunction->AddPoint(792, 57.8 / 255);
    opacityTransferFunction->AddPoint(1035, 0.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction); //���ò�͸���ȴ��亯��  

    //�����ݶȲ�͸������  
    //vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    //volumeGradientOpacity->AddPoint(0, 1);
    //volumeGradientOpacity->AddSegment(300, 0.73, 900, 0.9);
    //volumeGradientOpacity->AddPoint(1024, 0);
    //volumeProperty->SetGradientOpacity(volumeGradientOpacity);//�����ݶȲ�͸����Ч���Ա�  

     //������ɫ����  
     vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
     color->AddRGBPoint(0, 0, 0, 0);
     color->AddRGBPoint(55, 0, 0, 0);
     color->AddRGBPoint(140,164.0 / 255, 102.0 / 255, 19.0 / 255);
     color->AddRGBPoint(602, 250.0 / 255, 226.0 / 255, 110.0 / 255);
     color->AddRGBPoint(305.417, 207.0 / 255, 164.0 / 255, 201.0 / 255);
     color->AddRGBPoint(792, 251.0 / 255, 231.0 / 255, 210.0 / 255);
     color->AddRGBPoint(1035, 251.0 / 255, 231.0 / 255, 219.0 / 255);
     volumeProperty->SetColor(color);

    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    //������Ⱦ�Ĵ��ڣ�ָ��ΪvtkWin32OpenGLRenderWindow����vtkRenderer���������
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
    renderWindow->SetSize(iWidth, iHeight);              //���ô�С
    renderWindow->SetAAFrames(2);
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

