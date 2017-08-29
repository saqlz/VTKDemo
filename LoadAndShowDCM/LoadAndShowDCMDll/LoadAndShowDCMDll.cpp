// 这是主 DLL 文件。

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
    //Step0. 加Load DICOM Data
    //Step0. 加载DIOCM原始数据
    vtkSmartPointer<vtkDICOMImageReader> v16 = vtkSmartPointer<vtkDICOMImageReader>::New();
    v16->SetDataByteOrderToLittleEndian();
    v16->SetDirectoryName(sPath);
    v16->Update();

    //Step1. 加Load DICOM Data
    //Step1. 加载DIOCM原始数据
    vtkSmartPointer<vtkOutlineFilter> outlineData = vtkSmartPointer<vtkOutlineFilter>::New();
    outlineData->SetInputConnection(v16->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> mapOutline = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
    vtkSmartPointer<vtkActor> outline = vtkSmartPointer<vtkActor>::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(128, 128, 128);

    //设置渲染的窗口，指定为vtkWin32OpenGLRenderWindow，是vtkRenderer对象的容器
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
    renderWindow->SetSize(iWidth, iHeight);              //设置大小
    renderWindow->OffScreenRenderingOn();                //离屏渲染，不显示窗口

    //设置一个渲染的视野,一般称为ViewPort或者Cell
    //作用记录图像三维信息转换，聚合其他元素，比如照相机
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);                //注意一个vtkRenderWindow只能聚合一个vtkRenderer对象

    //设置照相机方位和位置
    vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();
    aCamera->SetViewUp(0, 0, -1);
    aCamera->SetPosition(0, 1, 0);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Zoom(0.5);
    aCamera->Azimuth(30.0);
    aCamera->Elevation(200.0);

    renderer->SetBackground(0, 0, 0);                   //设置背景为黑色
    renderer->AddActor(outline);                        //数据源
    renderer->SetActiveCamera(aCamera);                 //相机
    renderWindow->Render();                             //渲染，此时已经渲染出来了

                                                        //输出当前的PNG文件
    vtkSmartPointer<vtkWindowToImageFilter> windowToImage = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImage->SetInput(renderWindow);
    vtkSmartPointer<vtkPNGWriter> PNGWriter = vtkSmartPointer<vtkPNGWriter>::New();
    PNGWriter->SetInputConnection(windowToImage->GetOutputPort());
    PNGWriter->SetFileName("TestDelaunay2D.png");
    PNGWriter->Write();
    return 0;
}

