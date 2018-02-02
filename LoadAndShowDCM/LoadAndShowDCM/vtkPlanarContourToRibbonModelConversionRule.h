/*==============================================================================

Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
Queen's University, Kingston, ON, Canada. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file was originally developed by Csaba Pinter, PerkLab, Queen's University
and was supported through the Applied Cancer Research Unit program of Cancer Care
Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#ifndef __vtkPlanarContourToRibbonModelConversionRule_h
#define __vtkPlanarContourToRibbonModelConversionRule_h

#include "stdafx.h"
class vtkPolyData;
class vtkPoints;
class vtkCell;
class vtkPlane;

/// \ingroup DicomRtImportImportExportConversionRules
/// \brief Convert planar contour representation (vtkPolyData type) to ribbon
///   model representation (also vtkPolyData) by thickening the contours along
///   a normal vector orthogonal to the planes
class vtkPlanarContourToRibbonModelConversionRule
{
public:

    vtkPlanarContourToRibbonModelConversionRule();
    ~vtkPlanarContourToRibbonModelConversionRule();

    /// Update the target representation based on the source representation
    virtual bool Convert(vtkDataObject* sourceRepresentation, vtkDataObject* targetRepresentation);


protected:
    /// Compute plane for a given contour
    /// \param contourPoints Point list containing all the points (from which \sa contourCell selects a subset)
    /// \param contourCell Input contour cell, indices of the points in \sa contourPoints belonging to the contour
    /// \param contourPlane Computed contour plane (output argument)
    bool ComputePlaneForContour(vtkPoints* contourPoints, vtkCell* contourCell, vtkPlane* contourPlane);

    /// Determine the distance between contour planes based on the actual planar contour data
    /// \param planarContourPolyData Input poly data containing the planar contours
    /// \param contoursPlane Output argument for plane of the contours
    /// \return Computed plane spacing. 1mm in case of critical errors (so that the ribbon can be visualized in all cases)
    double ComputeContourPlaneSpacing(vtkPolyData* planarContourPolyData, vtkPlane* contoursPlane);

protected:
 

private:
    void operator=(const vtkPlanarContourToRibbonModelConversionRule&); // Not implemented
};

#endif // __vtkPlanarContourToRibbonModelConversionRule_h
