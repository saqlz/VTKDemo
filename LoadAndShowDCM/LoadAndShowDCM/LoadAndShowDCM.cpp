// LoadAndShowDCM.cpp : �������̨Ӧ�ó������ڵ㡣
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
    // Create the renderer, the render window, and the interactor. The
    // renderer draws into the render window, the interactor enables
    // mouse- and keyboard-based interaction with the data within the
    // render window.
    //
    vtkSmartPointer<vtkRenderer> aRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin =
        vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(aRenderer);
    vtkSmartPointer<vtkRenderWindowInteractor> iren =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    // Set a background color for the renderer and set the size of the
    // render window (expressed in pixels).
    aRenderer->SetBackground(0, 0, 0);
    renWin->SetSize(640, 480);

    // The following reader is used to read a series of 2D slices (images)
    // that compose the volume. The slice dimensions are set, and the
    // pixel spacing. The data Endianness must also be specified. The
    // reader uses the FilePrefix in combination with the slice number to
    // construct filenames using the format FilePrefix.%d. (In this case
    // the FilePrefix is the root name of the file: quarter.)
    vtkSmartPointer<vtkDICOMImageReader > v16 =
        vtkSmartPointer<vtkDICOMImageReader >::New();
    v16->SetDataByteOrderToLittleEndian();
    v16->SetDirectoryName(folder.c_str());
    v16->SetDataSpacing(0.974609375, 0.974609375, 5);
    v16->Update();
    // An isosurface, or contour value of 500 is known to correspond to the
    // skin of the patient. Once generated, a vtkPolyDataNormals filter is
    // is used to create normals for smooth surface shading during rendering.
    vtkSmartPointer<vtkContourFilter> skinExtractor =
        vtkSmartPointer<vtkContourFilter>::New();
    skinExtractor->SetInputConnection(v16->GetOutputPort());
    skinExtractor->SetValue(0, 0);

    vtkSmartPointer<vtkPolyDataNormals> skinNormals =
        vtkSmartPointer<vtkPolyDataNormals>::New();
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(0.0);

    vtkSmartPointer<vtkPolyDataMapper> skinMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    skinMapper->SetInputConnection(skinNormals->GetOutputPort());
    skinMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> skin =
        vtkSmartPointer<vtkActor>::New();
    skin->SetMapper(skinMapper);

    // An outline provides context around the data.
    //
    vtkSmartPointer<vtkOutlineFilter> outlineData =
        vtkSmartPointer<vtkOutlineFilter>::New();
    outlineData->SetInputConnection(v16->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapOutline =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());

    vtkSmartPointer<vtkActor> outline =
        vtkSmartPointer<vtkActor>::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(0, 0, 0);

    // It is convenient to create an initial view of the data. The FocalPoint
    // and Position form a vector direction. Later on (ResetCamera() method)
    // this vector is used to position the camera to look at the data in
    // this direction.
    vtkSmartPointer<vtkCamera> aCamera =
        vtkSmartPointer<vtkCamera>::New();
    aCamera->SetViewUp(0, 0, -1);
    aCamera->SetPosition(0, 1, 0);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);

    // Actors are added to the renderer. An initial camera view is created.
    // The Dolly() method moves the camera towards the FocalPoint,
    // thereby enlarging the image.
    aRenderer->AddActor(outline);
    aRenderer->AddActor(skin);
    aRenderer->SetActiveCamera(aCamera);
    aRenderer->ResetCamera();
    aCamera->Dolly(1.5);

    // Set a background color for the renderer and set the size of the
    // render window (expressed in pixels).
    aRenderer->SetBackground(0, 0, 0);
    renWin->SetSize(640, 480);

    // Note that when camera movement occurs (as it does in the Dolly()
    // method), the clipping planes often need adjusting. Clipping planes
    // consist of two planes: near and far along the view direction. The
    // near plane clips out objects in front of the plane; the far plane
    // clips out objects behind the plane. This way only what is drawn
    // between the planes is actually rendered.
    aRenderer->ResetCameraClippingRange();

    // Initialize the event loop and then start it.
    iren->Initialize();
    iren->Start();

    return 0;
}

