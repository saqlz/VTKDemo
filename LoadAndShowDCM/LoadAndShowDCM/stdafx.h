// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>


// TODO:  在此处引用程序需要的其他头文件
#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageViewer2.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMetaImageReader.h"
#include "vtkAutoInit.h"

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
