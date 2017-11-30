// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageViewer2.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMetaImageReader.h"
#include "vtkAutoInit.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkVolume16Reader.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineFilter.h"
#include "vtkGenericDataArray.txx"
#include "vtkStripper.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkImageMapper3D.h"
#include "vtkPNGWriter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkOpenGLGPUVolumeRayCastMapper.h"
#include "vtkMultiBlockVolumeMapper.h"
#include "vtkOpenGLProjectedTetrahedraMapper.h"
#include "vtkProperty.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h" 
#include "vtkImageBlend.h"
#include "vtkScalarBarActor.h"
#include "vtkRibbonFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkImageCast.h"

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);
