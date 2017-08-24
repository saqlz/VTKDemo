// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageViewer2.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int main()
{
    std::string folder = "E:\\Images\\Test\\";
    vtkSmartPointer<vtkDICOMImageReader> reader =
        vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName(folder.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageViewer2> viewer =
        vtkSmartPointer<vtkImageViewer2>::New();
    viewer->SetInputConnection(reader->GetOutputPort());

    //设置基本属性
    viewer->SetSize(640, 480);
    viewer->SetColorLevel(500);
    viewer->SetColorWindow(2000);
    viewer->SetSlice(40);
    viewer->SetSliceOrientationToXY();
    viewer->Render();
    viewer->GetRenderer()->SetBackground(1, 1, 1);
    viewer->GetRenderWindow()->SetWindowName("ImageViewer2D");

    vtkSmartPointer<vtkRenderWindowInteractor> rwi =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //设置交互属性
    viewer->SetupInteractor(rwi);

    rwi->Start();

    return 0;
}

