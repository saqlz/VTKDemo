// LoadAndShowDCM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*=========================================================================

Program:   Visualization Toolkit
Module:    ImageSlicing.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example shows how to load a 3D image into VTK and then reformat
// that image into a different orientation for viewing.  It uses
// vtkImageReslice for reformatting the image, and uses vtkImageActor
// and vtkInteractorStyleImage to display the image.  This InteractorStyle
// forces the camera to stay perpendicular to the XY plane.
//
// Thanks to David Gobbi of Atamai Inc. for contributing this example.
//

#include "vtkSmartPointer.h"
#include "vtkImageReader2.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageShrink3D.h" //降采样头文件  
#include "vtkImageMagnify.h" //升采样头文件 

// The mouse motion callback, to turn "Slicing" on and off
class vtkImageInteractionCallback : public vtkCommand
{
public:

    static vtkImageInteractionCallback *New() {
        return new vtkImageInteractionCallback;
    };

    vtkImageInteractionCallback() {
        this->Slicing = 0;
        this->ImageBlend = 0;
        this->Interactor = 0;
    };

    void SetImageReslice(vtkImageBlend *blend) {
        this->ImageBlend = blend;
    };

    vtkImageBlend *GetImageReslice() {
        return this->ImageBlend;
    };

    void SetInteractor(vtkRenderWindowInteractor *interactor) {
        this->Interactor = interactor;
    };

    vtkRenderWindowInteractor *GetInteractor() {
        return this->Interactor;
    };

    void Execute(vtkObject *, unsigned long event, void *) VTK_OVERRIDE
    {
        vtkRenderWindowInteractor *interactor = this->GetInteractor();

        int lastPos[2];
        interactor->GetLastEventPosition(lastPos);
        int currPos[2];
        interactor->GetEventPosition(currPos);

        if (event == vtkCommand::LeftButtonPressEvent)
        {
            this->Slicing = 1;
        }
        else if (event == vtkCommand::LeftButtonReleaseEvent)
        {
            this->Slicing = 0;
        }
        else if (event == vtkCommand::MouseMoveEvent)
        {
            if (this->Slicing)
            {
                vtkImageBlend *blend = this->ImageBlend;
                // Increment slice position by deltaY of mouse
                int deltaY = lastPos[1] - currPos[1];

                //reslice->Update();
//                 double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
//                 vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
//                 // move the center point that we are slicing through
//                 double point[4];
//                 double center[4];
//                 point[0] = 0.0;
//                 point[1] = 0.0;
//                 point[2] = sliceSpacing * deltaY;
//                 point[3] = 1.0;
//                 matrix->MultiplyPoint(point, center);
//                 matrix->SetElement(0, 3, center[0]);
//                 matrix->SetElement(1, 3, center[1]);
//                 matrix->SetElement(2, 3, center[2]);
                interactor->Render();
            }
            else
            {
                vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
                    interactor->GetInteractorStyle());
                if (style)
                {
                    style->OnMouseMove();
                }
            }
        }
    };

private:

    // Actions (slicing only, for now)
    int Slicing;

    // Pointer to vtkImageReslice
    vtkImageBlend *ImageBlend;

    // Pointer to the interactor
    vtkRenderWindowInteractor *Interactor;
};


const double color[256][3] = { { 0 / 255.0, 0 / 255.0, 0 / 255.0 },
{ 4 / 255.0, 0 / 255.0, 2 / 255.0 },
{ 8 / 255.0, 0 / 255.0, 4 / 255.0 },
{ 12 / 255.0, 0 / 255.0, 6 / 255.0 },
{ 16 / 255.0, 0 / 255.0, 8 / 255.0 },
{ 20 / 255.0, 0 / 255.0, 10 / 255.0 },
{ 24 / 255.0, 0 / 255.0, 12 / 255.0 },
{ 28 / 255.0, 0 / 255.0, 14 / 255.0 },
{ 32 / 255.0, 0 / 255.0, 16 / 255.0 },
{ 36 / 255.0, 0 / 255.0, 18 / 255.0 },
{ 40 / 255.0, 0 / 255.0, 20 / 255.0 },
{ 44 / 255.0, 0 / 255.0, 22 / 255.0 },
{ 48 / 255.0, 0 / 255.0, 24 / 255.0 },
{ 52 / 255.0, 0 / 255.0, 26 / 255.0 },
{ 56 / 255.0, 0 / 255.0, 28 / 255.0 },
{ 60 / 255.0, 0 / 255.0, 30 / 255.0 },
{ 64 / 255.0, 0 / 255.0, 32 / 255.0 },
{ 68 / 255.0, 0 / 255.0, 34 / 255.0 },
{ 72 / 255.0, 0 / 255.0, 36 / 255.0 },
{ 76 / 255.0, 0 / 255.0, 38 / 255.0 },
{ 80 / 255.0, 0 / 255.0, 40 / 255.0 },
{ 84 / 255.0, 0 / 255.0, 42 / 255.0 },
{ 88 / 255.0, 0 / 255.0, 44 / 255.0 },
{ 92 / 255.0, 0 / 255.0, 46 / 255.0 },
{ 96 / 255.0, 0 / 255.0, 48 / 255.0 },
{ 100 / 255.0, 0 / 255.0, 50 / 255.0 },
{ 104 / 255.0, 0 / 255.0, 52 / 255.0 },
{ 108 / 255.0, 0 / 255.0, 54 / 255.0 },
{ 112 / 255.0, 0 / 255.0, 56 / 255.0 },
{ 116 / 255.0, 0 / 255.0, 58 / 255.0 },
{ 120 / 255.0, 0 / 255.0, 60 / 255.0 },
{ 124 / 255.0, 0 / 255.0, 62 / 255.0 },
{ 128 / 255.0, 0 / 255.0, 64 / 255.0 },
{ 132 / 255.0, 0 / 255.0, 62 / 255.0 },
{ 136 / 255.0, 0 / 255.0, 60 / 255.0 },
{ 140 / 255.0, 0 / 255.0, 58 / 255.0 },
{ 144 / 255.0, 0 / 255.0, 56 / 255.0 },
{ 148 / 255.0, 0 / 255.0, 54 / 255.0 },
{ 152 / 255.0, 0 / 255.0, 52 / 255.0 },
{ 156 / 255.0, 0 / 255.0, 50 / 255.0 },
{ 160 / 255.0, 0 / 255.0, 48 / 255.0 },
{ 164 / 255.0, 0 / 255.0, 46 / 255.0 },
{ 168 / 255.0, 0 / 255.0, 44 / 255.0 },
{ 172 / 255.0, 0 / 255.0, 42 / 255.0 },
{ 176 / 255.0, 0 / 255.0, 40 / 255.0 },
{ 180 / 255.0, 0 / 255.0, 38 / 255.0 },
{ 184 / 255.0, 0 / 255.0, 36 / 255.0 },
{ 188 / 255.0, 0 / 255.0, 34 / 255.0 },
{ 192 / 255.0, 0 / 255.0, 32 / 255.0 },
{ 196 / 255.0, 0 / 255.0, 30 / 255.0 },
{ 200 / 255.0, 0 / 255.0, 28 / 255.0 },
{ 204 / 255.0, 0 / 255.0, 26 / 255.0 },
{ 208 / 255.0, 0 / 255.0, 24 / 255.0 },
{ 212 / 255.0, 0 / 255.0, 22 / 255.0 },
{ 216 / 255.0, 0 / 255.0, 20 / 255.0 },
{ 220 / 255.0, 0 / 255.0, 18 / 255.0 },
{ 224 / 255.0, 0 / 255.0, 16 / 255.0 },
{ 228 / 255.0, 0 / 255.0, 14 / 255.0 },
{ 232 / 255.0, 0 / 255.0, 12 / 255.0 },
{ 236 / 255.0, 0 / 255.0, 10 / 255.0 },
{ 240 / 255.0, 0 / 255.0, 8 / 255.0 },
{ 244 / 255.0, 0 / 255.0, 6 / 255.0 },
{ 248 / 255.0, 0 / 255.0, 4 / 255.0 },
{ 252 / 255.0, 0 / 255.0, 2 / 255.0 },
{ 255 / 255.0, 0 / 255.0, 0 / 255.0 },
{ 247 / 255.0, 8 / 255.0, 0 / 255.0 },
{ 239 / 255.0, 16 / 255.0, 0 / 255.0 },
{ 231 / 255.0, 24 / 255.0, 0 / 255.0 },
{ 223 / 255.0, 32 / 255.0, 0 / 255.0 },
{ 215 / 255.0, 40 / 255.0, 0 / 255.0 },
{ 207 / 255.0, 48 / 255.0, 0 / 255.0 },
{ 199 / 255.0, 56 / 255.0, 0 / 255.0 },
{ 191 / 255.0, 64 / 255.0, 0 / 255.0 },
{ 183 / 255.0, 72 / 255.0, 0 / 255.0 },
{ 175 / 255.0, 80 / 255.0, 0 / 255.0 },
{ 167 / 255.0, 88 / 255.0, 0 / 255.0 },
{ 159 / 255.0, 96 / 255.0, 0 / 255.0 },
{ 151 / 255.0, 104 / 255.0, 0 / 255.0 },
{ 143 / 255.0, 112 / 255.0, 0 / 255.0 },
{ 135 / 255.0, 120 / 255.0, 0 / 255.0 },
{ 127 / 255.0, 128 / 255.0, 0 / 255.0 },
{ 119 / 255.0, 136 / 255.0, 0 / 255.0 },
{ 111 / 255.0, 144 / 255.0, 0 / 255.0 },
{ 103 / 255.0, 152 / 255.0, 0 / 255.0 },
{ 95 / 255.0, 160 / 255.0, 0 / 255.0 },
{ 87 / 255.0, 168 / 255.0, 0 / 255.0 },
{ 79 / 255.0, 176 / 255.0, 0 / 255.0 },
{ 71 / 255.0, 184 / 255.0, 0 / 255.0 },
{ 63 / 255.0, 192 / 255.0, 0 / 255.0 },
{ 55 / 255.0, 200 / 255.0, 0 / 255.0 },
{ 47 / 255.0, 208 / 255.0, 0 / 255.0 },
{ 39 / 255.0, 216 / 255.0, 0 / 255.0 },
{ 31 / 255.0, 224 / 255.0, 0 / 255.0 },
{ 23 / 255.0, 232 / 255.0, 0 / 255.0 },
{ 15 / 255.0, 240 / 255.0, 0 / 255.0 },
{ 7 / 255.0, 248 / 255.0, 0 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 0 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 4 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 8 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 12 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 16 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 20 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 24 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 28 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 32 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 36 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 40 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 44 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 48 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 52 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 56 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 60 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 64 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 68 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 72 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 76 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 80 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 84 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 88 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 92 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 96 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 100 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 104 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 108 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 112 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 116 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 120 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 124 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 128 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 132 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 136 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 140 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 144 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 148 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 152 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 156 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 160 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 164 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 168 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 172 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 176 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 180 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 184 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 188 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 192 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 196 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 200 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 204 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 208 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 212 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 216 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 220 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 224 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 228 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 232 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 236 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 240 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 244 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 248 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 252 / 255.0 },
{ 0 / 255.0, 255 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 253 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 251 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 249 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 247 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 245 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 243 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 241 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 239 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 237 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 235 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 233 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 231 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 229 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 227 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 225 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 223 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 221 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 219 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 217 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 215 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 213 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 211 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 209 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 207 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 205 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 203 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 201 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 199 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 197 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 195 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 193 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 192 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 189 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 186 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 183 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 180 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 177 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 174 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 171 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 168 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 165 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 162 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 159 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 156 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 153 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 150 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 147 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 144 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 141 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 138 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 135 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 132 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 129 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 126 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 123 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 120 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 117 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 114 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 111 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 108 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 105 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 102 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 99 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 96 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 93 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 90 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 87 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 84 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 81 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 78 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 75 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 72 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 69 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 66 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 63 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 60 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 57 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 54 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 51 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 48 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 45 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 42 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 39 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 36 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 33 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 30 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 27 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 24 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 21 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 18 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 15 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 12 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 9 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 6 / 255.0, 255 / 255.0 },
{ 0 / 255.0, 3 / 255.0, 255 / 255.0 } };

namespace
{
    template<typename T>
    T* LoadRawVolume(int(&iDim)[3], const std::string &strPath)
    {
        std::ifstream in(strPath, std::ios::binary | std::ios::in);
        if (!in.is_open())
        {
            std::cout << "Can't open file : " << strPath << std::endl;
            return nullptr;
        }

        T* pData = new T[4 * iDim[0] * iDim[1] * iDim[2]];
        in.read((char*)pData, 4 * iDim[0] * iDim[1] * iDim[2] * sizeof(T));
        in.close();

        return pData;
    }
}

// The program entry point
int main()
{
    // Matrices for axial, coronal, sagittal, oblique view orientations
    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    std::string sPath = "E:\\Images\\Test\\";
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDataByteOrderToLittleEndian();
    reader->SetDirectoryName(sPath.c_str());
    reader->Update();
    int extent[6];
    double spacing[3];
    double origin[3];
    reader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);
    double centerBottom[3];
    centerBottom[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    centerBottom[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    centerBottom[2] = origin[2] + spacing[2] * 0.42 * (extent[4] + extent[5]);

    vtkSmartPointer<vtkMatrix4x4> resliceAxesBottom =
        vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesBottom->DeepCopy(axialElements);
    resliceAxesBottom->SetElement(0, 3, centerBottom[0]);
    resliceAxesBottom->SetElement(1, 3, centerBottom[1]);
    resliceAxesBottom->SetElement(2, 3, centerBottom[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceBottom =
        vtkSmartPointer<vtkImageReslice>::New();
    resliceBottom->SetInputConnection(reader->GetOutputPort());
    resliceBottom->SetOutputDimensionality(2);
    resliceBottom->SetResliceAxes(resliceAxesBottom);
    resliceBottom->SetInterpolationModeToLinear();

    vtkSmartPointer<vtkLookupTable> tableBottom = vtkSmartPointer<vtkLookupTable>::New();
    tableBottom->SetRange(-355, 300); // image intensity range
    tableBottom->SetValueRange(0.0, 1.0); // from black to white
    tableBottom->SetSaturationRange(0.0, 0.0); // no color saturation
    tableBottom->SetRampToLinear();
    tableBottom->Build();

    vtkSmartPointer<vtkImageMapToColors> mapBottom = vtkSmartPointer<vtkImageMapToColors>::New();
    mapBottom->SetInputConnection(resliceBottom->GetOutputPort());
    mapBottom->SetLookupTable(tableBottom);
    mapBottom->Update();

    //DOSE信息
    static double axialElementsTop[16] = {
        1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };
    std::string fDoseFilePath = "D:\\GitHub\\WisdomRay\\appdata\\dose.dat";
    int iVolumeDimension[3] = { 150, 138, 198 };
    int iComponent = 4;
    unsigned char* imageData = LoadRawVolume<unsigned char>(iVolumeDimension, fDoseFilePath);
    vtkSmartPointer<vtkImageData> doseImageData = vtkSmartPointer<vtkImageData>::New();
    doseImageData->SetDimensions(iVolumeDimension);
    doseImageData->AllocateScalars(VTK_UNSIGNED_CHAR, iComponent);
    doseImageData->SetOrigin(0, 0, 0);
    doseImageData->SetSpacing(2.5390625, 2.5390625, 2);
    unsigned char* ptr = reinterpret_cast<unsigned char*>(doseImageData->GetScalarPointer());
    for (int i = 0; i < iVolumeDimension[0] * iVolumeDimension[1] * iVolumeDimension[2] * iComponent; i += iComponent)
    {
        ptr[i] = imageData[i];
        ptr[i + 1] = imageData[i + 1];
        ptr[i + 2] = imageData[i + 2];
        ptr[i + 3] = imageData[i + 3];
    }

    //重采样
    vtkSmartPointer<vtkImageMagnify> magnifyFilter = vtkSmartPointer<vtkImageMagnify>::New();
    magnifyFilter->SetInputData(doseImageData);
    magnifyFilter->SetMagnificationFactors(2.5390625 / spacing[0], 2.5390625 / spacing[0], 1);
    magnifyFilter->Update();

    // Set the slice orientation
    vtkSmartPointer<vtkMatrix4x4> resliceAxesTop = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxesTop->DeepCopy(axialElementsTop);
    resliceAxesTop->SetElement(0, 3, 150);
    resliceAxesTop->SetElement(1, 3, 138);
    resliceAxesTop->SetElement(2, 3, centerBottom[2]);

    // Extract a slice in the desired orientation
    vtkSmartPointer<vtkImageReslice> resliceTop = vtkSmartPointer<vtkImageReslice>::New();
    resliceTop->SetInputData(magnifyFilter->GetOutput());
    resliceTop->SetOutputDimensionality(2);
    resliceTop->SetResliceAxes(resliceAxesTop);
    resliceTop->SetInterpolationModeToLinear();

    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> tableTop = vtkSmartPointer<vtkLookupTable>::New();
    tableTop->SetNumberOfColors(256);
    for (int i = 0; i < 256; i++)
    {
        tableTop->SetTableValue(i, color[i][0], color[i][1], color[i][2]);
    }
    tableTop->SetRange(10, 150);
    tableTop->Build();

    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> colorTop = vtkSmartPointer<vtkImageMapToColors>::New();
    colorTop->SetLookupTable(tableTop);
    colorTop->SetInputConnection(resliceTop->GetOutputPort());
    colorTop->Update();




    int originalDims[3];
    reader->GetOutput()->GetDimensions(originalDims);
    double originalSpacing[3];
    reader->GetOutput()->GetSpacing(originalSpacing);
    int shrinkDims[3];
    int magnifyDims[3];
    magnifyFilter->GetOutput()->GetDimensions(magnifyDims);
    double magnifuSpacing[3];
    magnifyFilter->GetOutput()->GetSpacing(magnifuSpacing);
    std::cout << "原始维度：      " << originalDims[0] << "*" << originalDims[1] << "*" << originalDims[2] << std::endl;
    std::cout << "原始像素间距：  " << originalSpacing[0] << " " << originalSpacing[1] << " " << originalSpacing[2] << std::endl;
    std::cout << "升采样维度：    " << magnifyDims[0] << "*" << magnifyDims[1] << "*" << magnifyDims[2] << std::endl;
    std::cout << "升采样像素间距：" << magnifuSpacing[0] << " " << magnifuSpacing[1] << " " << magnifuSpacing[2] << std::endl;


    vtkSmartPointer<vtkImageCanvasSource2D> drawing = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    drawing->SetNumberOfScalarComponents(4);
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetExtent(extent);
    drawing->SetDrawColor(0.0, 0.0, 0.0);
    drawing->FillBox(extent[0], extent[1], extent[2], extent[3]);
    drawing->DrawImage(508.365234375 - 423.9414063, 324.365234375 - 189.1601563, colorTop->GetOutput());
    drawing->Update();

    vtkSmartPointer<vtkImageBlend> blend = vtkSmartPointer<vtkImageBlend>::New();
    blend->SetBlendModeToNormal();
    blend->AddInputData(mapBottom->GetOutput());
    blend->AddInputData(drawing->GetOutput());
    blend->SetOpacity(0, 0.5);
    blend->SetOpacity(1, 0.5);
    blend->Update();

    // Display the image
    vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
    actor->SetInputData(blend->GetOutput());
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);

    // Set up the interaction
    vtkSmartPointer<vtkInteractorStyleImage> imageStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetInteractorStyle(imageStyle);
    window->SetInteractor(interactor);
    window->Render();

//     vtkSmartPointer<vtkImageInteractionCallback> callback = vtkSmartPointer<vtkImageInteractionCallback>::New();
//     callback->SetImageReslice(blend);
//     callback->SetInteractor(interactor);
// 
//     imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
//     imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
//     imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

    // Start interaction
    // The Start() method doesn't return until the window is closed by the user
    interactor->Start();

    return EXIT_SUCCESS;
}

