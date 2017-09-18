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
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDataByteOrderToLittleEndian();
    reader->SetDirectoryName(sPath.c_str());
    reader->Update();


    int* extent;
    double spacing[3];
    double origin[3];


    extent = reader->GetDataExtent();
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);

    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    static double sagittalElements[16] = {
        0, 0,-1, 0,
        1, 0, 0, 0,
        0,-1, 0, 0,
        0, 0, 0, 1 };

    vtkSmartPointer<vtkMatrix4x4> resliceAxes =  vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxes->DeepCopy(sagittalElements);
    // Set the point through which to slice
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);

    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputConnection(reader->GetOutputPort());
    reslice->SetOutputDimensionality(3);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();
    
    vtkSmartPointer<vtkLookupTable> table =
        vtkSmartPointer<vtkLookupTable>::New();
    table->SetRange(-1024, 3000);        // image intensity range
    table->SetValueRange(0.0, 1.0);      // from black to white
    table->SetSaturationRange(0.0, 0.0); // no color saturation
    table->SetRampToLinear();
    table->Build();


    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> color1 =
        vtkSmartPointer<vtkImageMapToColors>::New();
    color1->SetLookupTable(table);
    color1->SetInputConnection(reslice->GetOutputPort());



    vtkSmartPointer<vtkImageActor> imageActor =
        vtkSmartPointer<vtkImageActor>::New();
    imageActor->GetMapper()->SetInputConnection(color1->GetOutputPort());

    
    //选择体绘制算法
    vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(reader->GetOutput());
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

                                       //设置不透明度与WWWL调窗一致 
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(55, 0.0);
    opacityTransferFunction->AddPoint(140, 164.721 / 255);
    opacityTransferFunction->AddPoint(305.417, 231.458 / 255);
    opacityTransferFunction->AddPoint(602, 235.0 / 255);
    opacityTransferFunction->AddPoint(792, 57.8 / 255);
    opacityTransferFunction->AddPoint(1035, 0.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction); //设置不透明度传输函数  

    //设置梯度不透明属性  
    //vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    //volumeGradientOpacity->AddPoint(0, 1);
    //volumeGradientOpacity->AddSegment(300, 0.73, 900, 0.9);
    //volumeGradientOpacity->AddPoint(1024, 0);
    //volumeProperty->SetGradientOpacity(volumeGradientOpacity);//设置梯度不透明度效果对比  

    //设置颜色属性  
    vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
    color->AddRGBPoint(0, 0, 0, 0);
    color->AddRGBPoint(55, 0, 0, 0);
    color->AddRGBPoint(140, 164.0 / 255, 102.0 / 255, 19.0 / 255);
    color->AddRGBPoint(602, 250.0 / 255, 226.0 / 255, 110.0 / 255);
    color->AddRGBPoint(305.417, 207.0 / 255, 164.0 / 255, 201.0 / 255);
    color->AddRGBPoint(792, 251.0 / 255, 231.0 / 255, 210.0 / 255);
    color->AddRGBPoint(1035, 251.0 / 255, 231.0 / 255, 219.0 / 255);
    volumeProperty->SetColor(color);

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
    /*aCamera->SetViewUp(0, 0, -1);
    aCamera->SetPosition(0, 1, 0);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);*/

    renderer->SetBackground(0, 0, 0);                   //设置背景为黑色
    renderer->AddVolume(volume);                        //数据源
    renderer->AddActor(imageActor);

    renderer->SetActiveCamera(aCamera);                 //相机
    renderer->ResetCamera();
    renderWindow->Render();                             //渲染，此时已经渲染出来了
    vtkSmartPointer<vtkRenderWindowInteractor> rwi = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    rwi->SetRenderWindow(renderWindow);
    rwi->Start();
    return 0;
}

