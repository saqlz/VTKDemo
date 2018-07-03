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
#include <map>


// TODO:  在此处引用程序需要的其他头文件
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
#include "vtkImageStencilToImage.h"
#include "vtkImageAccumulate.h"
#include "vtkAppendPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkFloatArray.h"
#include "vtkCleanPolyData.h"
#include "vtkCellData.h"
#include "vtkMarchingSquares.h"
#include "vtkMarchingContourFilter.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"
#include "vtkCutter.h"
#include "vtkGeneralTransform.h"
#include "vtkCutter.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPlane.h"
#include "vtkTransform.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageWrapPad.h"
#include "vtkImageCheckerboard.h"
#include "vtkImageExtractComponents.h"
#include "vtkXYPlotActor.h"
#include "vtkPolyData.h"
#include "vtkParallelCoordinatesView.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkParallelCoordinatesView.h>
#include <vtkParallelCoordinatesRepresentation.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkNew.h"

#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkImageSliceMapper.h"
#include "vtkImageProperty.h"
#include "vtkImageSlice.h"
#include "vtkPNGReader.h"
#include "vtkImageHistogram.h"
#include "vtkBarChartActor.h"
#include "vtkProperty2D.h"
#include "vtkLegendBoxActor.h"

#include "itkVectorImage.h"
#include "itkImageToHistogramFilter.h"
#include "itkImageRandomIteratorWithIndex.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
//#include "itkContourExtractor2DImageFilter.h"
#include "itkApproximateSignedDistanceMapImageFilter.h"
#include "vtkImageHistogram.h"

#include "vtkPolyLine.h"
#include "vtkImageGridSource.h"
#include "vtkHyperTreeGridSource.h"
#include "vtkHyperTreeGridGeometry.h"

#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridGeometry.h"
#include "vtkHyperTreeGridSource.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkLineSource.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyLine.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkRectilinearGrid.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkStructuredGrid.h"
#include "vtkStructuredGridGeometryFilter.h"
#include "vtkTextActor.h"



VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);
