// ������ DLL �ļ���

#include "stdafx.h"
#include "LoadAndShowDCMDll.h"

int LoadAndShowDCM(const char* sPath, int iWidth, int iHeight)
{
    WisdomTechLoadAndShowDCM *instance = new WisdomTechLoadAndShowDCM();
    int i = instance->LoadAndShowByPathAndDim(sPath, iWidth, iHeight);
    delete instance;
    return i;
}


WisdomTechLoadAndShowDCM::WisdomTechLoadAndShowDCM()
{
}

WisdomTechLoadAndShowDCM::~WisdomTechLoadAndShowDCM()
{
}

int WisdomTechLoadAndShowDCM::LoadAndShowByPathAndDim(const char* sPath, const int& iWidth, const int& iHeight)
{
    //Step0. ��Load DICOM Data
    //Step0. ����DIOCMԭʼ����
    vtkSmartPointer<vtkDICOMImageReader> v16 = vtkSmartPointer<vtkDICOMImageReader>::New();
    v16->SetDataByteOrderToLittleEndian();
    v16->SetDirectoryName(sPath);
    v16->Update();

    //Step1. ��Load DICOM Data
    //Step1. ����DIOCMԭʼ����
    vtkSmartPointer<vtkOutlineFilter> outlineData = vtkSmartPointer<vtkOutlineFilter>::New();
    outlineData->SetInputConnection(v16->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> mapOutline = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
    vtkSmartPointer<vtkActor> outline = vtkSmartPointer<vtkActor>::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(128, 128, 128);

    //������Ⱦ�Ĵ��ڣ�ָ��ΪvtkWin32OpenGLRenderWindow����vtkRenderer���������
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
    renderWindow->SetSize(iWidth, iHeight);              //���ô�С
    renderWindow->OffScreenRenderingOn();                //������Ⱦ������ʾ����

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
    aCamera->Zoom(0.5);
    aCamera->Azimuth(30.0);
    aCamera->Elevation(200.0);

    renderer->SetBackground(0, 0, 0);                   //���ñ���Ϊ��ɫ
    renderer->AddActor(outline);                        //����Դ
    renderer->SetActiveCamera(aCamera);                 //���
    renderWindow->Render();                             //��Ⱦ����ʱ�Ѿ���Ⱦ������
   
    vtkSmartPointer<vtkWindowToImageFilter> windowToImage = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImage->SetInput(renderWindow);
    windowToImage->Update();
    vtkSmartPointer<vtkImageData> imageData = windowToImage->GetOutput();
    void* image = imageData->GetScalarPointer();
    int* imageDimension = imageData->GetDimensions();
    int component = imageData->GetNumberOfScalarComponents();

    std::string host = "127.0.0.1";
    vtkSmartPointer<vtkClientSocket> socketCommunicator = vtkSmartPointer<vtkClientSocket>::New();
    socketCommunicator->ConnectToServer(host.c_str(), 20000);
    socketCommunicator->Send(image, imageDimension[0] * imageDimension[1] * component);

    return 0;
}

