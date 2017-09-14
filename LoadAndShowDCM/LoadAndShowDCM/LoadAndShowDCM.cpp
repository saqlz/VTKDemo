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
   
    //���ò�͸����  
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(-324, 0, 0.5, 0.0);
    opacityTransferFunction->AddPoint(-16, 0, .49, .61);
    opacityTransferFunction->AddSegment(641, .72, .5, 0.0);
    opacityTransferFunction->AddPoint(3071, 0.0, 0.5, 0.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction); //���ò�͸���ȴ��亯��  

    //�����ݶȲ�͸������  
    vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    volumeGradientOpacity->AddPoint(0, 2.0);
    volumeGradientOpacity->AddPoint(-1024, 2.0);
    volumeGradientOpacity->AddSegment(600, 0.73, 900, 0.9);
    volumeGradientOpacity->AddPoint(1024, 0.1);
    volumeProperty->SetGradientOpacity(volumeGradientOpacity);//�����ݶȲ�͸����Ч���Ա�  

//     //������ɫ����  
//     vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
//     color->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
//     color->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
//     color->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
//     color->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);
//     volumeProperty->SetColor(color);

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

