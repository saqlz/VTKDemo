// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

int main()
{
    std::string sPath = "E:\\Images\\Test\\";
    int iWidth = 640;
    int iHeight = 480;

    //Step0. 加Load DICOM Data
    //Step0. 加载DIOCM原始数据
    vtkSmartPointer<vtkDICOMImageReader> v16 = vtkSmartPointer<vtkDICOMImageReader>::New();
    v16->SetDataByteOrderToLittleEndian();
    v16->SetDirectoryName(sPath.c_str());
    v16->Update();
  
    //选择体绘制算法
    vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(v16->GetOutput());
    //设置光线采样距离  
    //volumeMapper->SetSampleDistance(volumeMapper->GetSampleDistance()*4);  
    //设置图像采样步长  
    //volumeMapper->SetAutoAdjustSampleDistances(0);  
    //volumeMapper->SetImageSampleDistance(4);  
    /*************************************************************************/

    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();  //打开或者关闭阴影测试  
    volumeProperty->SetAmbient(0.4);   //环境光
    volumeProperty->SetDiffuse(0.6);   //漫反射
    volumeProperty->SetSpecular(0.5);  //镜面反射
   
    //设置不透明度  
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(-324, 0, 0.5, 0.0);
    opacityTransferFunction->AddPoint(-16, 0, .49, .61);
    opacityTransferFunction->AddSegment(641, .72, .5, 0.0);
    opacityTransferFunction->AddPoint(3071, 0.0, 0.5, 0.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction); //设置不透明度传输函数  

    //设置梯度不透明属性  
    vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    volumeGradientOpacity->AddPoint(0, 2.0);
    volumeGradientOpacity->AddPoint(-1024, 2.0);
    volumeGradientOpacity->AddSegment(600, 0.73, 900, 0.9);
    volumeGradientOpacity->AddPoint(1024, 0.1);
    volumeProperty->SetGradientOpacity(volumeGradientOpacity);//设置梯度不透明度效果对比  

//     //设置颜色属性  
//     vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
//     color->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
//     color->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
//     color->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
//     color->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);
//     volumeProperty->SetColor(color);

    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    //设置渲染的窗口，指定为vtkWin32OpenGLRenderWindow，是vtkRenderer对象的容器
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
    renderWindow->SetSize(iWidth, iHeight);              //设置大小
    renderWindow->SetAAFrames(2);
    //renderWindow->OffScreenRenderingOn();                //离屏渲染，不显示窗口

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
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);

    renderer->SetBackground(0, 0, 0);                   //设置背景为黑色
    renderer->AddVolume(volume);                        //数据源
    renderer->SetActiveCamera(aCamera);                 //相机
    renderer->ResetCamera();
    renderWindow->Render();                             //渲染，此时已经渲染出来了
    vtkSmartPointer<vtkRenderWindowInteractor> rwi =  vtkSmartPointer<vtkRenderWindowInteractor>::New();
    rwi->SetRenderWindow(renderWindow);
    rwi->Start();
    return 0;
}

