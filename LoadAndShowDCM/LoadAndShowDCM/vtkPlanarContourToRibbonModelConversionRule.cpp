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

// Segmentations includes
#include "StdAfx.h"
#include "vtkPlanarContourToRibbonModelConversionRule.h"
// VTK includes
#include <vtkObjectFactory.h>
#include <vtkVector.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkIdList.h>
#include <vtkPlane.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkRibbonFilter.h>
#include <vtkMath.h>
#include <sstream>
#define EPSILON 0.0001

//----------------------------------------------------------------------------
// Utility functions
namespace
{
    vtkVector3d operator -(const vtkVector3d& a, const vtkVector3d& b)
    {
        vtkVector3d result;
        result.SetX(a.GetX() - b.GetX());
        result.SetY(a.GetY() - b.GetY());
        result.SetZ(a.GetZ() - b.GetZ());
        return result;
    }

    bool AreEqualWithTolerance(double a, double b)
    {
        return fabs(a - b) < EPSILON;
    }

    double MajorityValue(const std::vector<double>& spacingValues, std::string &outputMessage)
    {
        std::map<double, int> spacingValueFrequencies;
        double averageTotal(0.0);
        double averageCount(0.0);
        for (std::vector<double>::const_iterator it = spacingValues.begin(); it != spacingValues.end(); ++it)
        {
            double roundedSpacingValue = vtkMath::Round((*it) / EPSILON) * EPSILON; // Round value to consider spacings within tolerance the same
            averageTotal += *it;
            averageCount++;
            spacingValueFrequencies[roundedSpacingValue]++;
        }
        double majorityValue(0.0);
        int majorityCount(-1);
        std::stringstream outputStringStream;
        for (std::map<double, int>::iterator it = spacingValueFrequencies.begin(); it != spacingValueFrequencies.end(); ++it)
        {
            outputStringStream << "  Planar spacing: " << it->first << ". Frequency: " << it->second << "." << std::endl;
            if (it->second > majorityCount)
            {
                majorityValue = it->first;
                majorityCount = it->second;
            }
        }
        outputMessage = outputStringStream.str();
        return majorityValue;
    }
}


vtkPlanarContourToRibbonModelConversionRule::vtkPlanarContourToRibbonModelConversionRule()
{

}

vtkPlanarContourToRibbonModelConversionRule::~vtkPlanarContourToRibbonModelConversionRule()
{

}

//----------------------------------------------------------------------------
bool vtkPlanarContourToRibbonModelConversionRule::Convert(vtkDataObject* sourceRepresentation, vtkDataObject* targetRepresentation)
{
    // Check validity of source and target representation objects
    vtkPolyData* planarContourPolyData = vtkPolyData::SafeDownCast(sourceRepresentation);
    if (!planarContourPolyData)
    {
        return false;
    }
    vtkPolyData* ribbonModelPolyData = vtkPolyData::SafeDownCast(targetRepresentation);
    if (!ribbonModelPolyData)
    {
        return false;
    }
    if (planarContourPolyData->GetNumberOfPoints() < 3 || planarContourPolyData->GetNumberOfCells() < 1)
    {
        return false;
    }

    // Compute plane spacing of contours
    vtkSmartPointer<vtkPlane> contoursPlane = vtkSmartPointer<vtkPlane>::New();
    double sliceThickness = ComputeContourPlaneSpacing(planarContourPolyData, contoursPlane);

    // Remove coincident points (if there are multiple contour points at the same position then the ribbon filter fails)
    vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
    cleaner->SetInputData(planarContourPolyData);
    cleaner->Update();

    // Convert to ribbon using vtkRibbonFilter
    vtkSmartPointer<vtkRibbonFilter> ribbonFilter = vtkSmartPointer<vtkRibbonFilter>::New();
    ribbonFilter->SetInputConnection(cleaner->GetOutputPort());
    ribbonFilter->SetDefaultNormal(contoursPlane->GetNormal());
    ribbonFilter->UseDefaultNormalOn();
    ribbonFilter->SetWidth(sliceThickness / 2.0);
    ribbonFilter->SetAngle(90.0);
    ribbonFilter->Update();

    vtkSmartPointer<vtkPolyDataNormals> normalFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
    normalFilter->SetInputConnection(ribbonFilter->GetOutputPort());
    normalFilter->ConsistencyOn();
    normalFilter->Update();

    ribbonModelPolyData->DeepCopy(normalFilter->GetOutput());

    return true;
}

//----------------------------------------------------------------------------
bool vtkPlanarContourToRibbonModelConversionRule::ComputePlaneForContour(vtkPoints* contourPoints, vtkCell* contourCell, vtkPlane* contourPlane)
{
    if (!contourPoints || !contourCell || !contourPlane)
    {
        return false;
    }

    // Get point indices
    vtkIdList* currentContourPointIds = contourCell->GetPointIds();
    int numberOfPoints = currentContourPointIds->GetNumberOfIds();
    if (numberOfPoints < 3)
    {
        return false;
    }

    // Compute equation of the current plane from the first point, and two other points about equal number of points away from the first one.
    // This heuristic method attempts to get an as accurate plane normal vector as possible by maximizing expected distance between plane points.
    int firstPointIndex = currentContourPointIds->GetId(0);
    int secondPointIndex = currentContourPointIds->GetId(numberOfPoints / 3);
    int thirdPointIndex = currentContourPointIds->GetId(2 * numberOfPoints / 3);
    // Sanity check of indices
    if (secondPointIndex == firstPointIndex || secondPointIndex == thirdPointIndex)
    {
        return false;
    }

    // Get plane points
    double firstPointArray[3] = { 0.0,0.0,0.0 };
    contourPoints->GetPoint(firstPointIndex, firstPointArray);
    double secondPointArray[3] = { 0.0,0.0,0.0 };
    contourPoints->GetPoint(secondPointIndex, secondPointArray);
    double thirdPointArray[3] = { 0.0,0.0,0.0 };
    contourPoints->GetPoint(thirdPointIndex, thirdPointArray);
    vtkVector3d firstPlanePoint(firstPointArray[0], firstPointArray[1], firstPointArray[2]);
    vtkVector3d secondPlanePoint(secondPointArray[0], secondPointArray[1], secondPointArray[2]);
    vtkVector3d thirdPlanePoint(thirdPointArray[0], thirdPointArray[1], thirdPointArray[2]);
    // Compute plane vectors: two vectors on the plane, and the normal
    vtkVector3d currentPlaneIVector(secondPlanePoint - firstPlanePoint);
    vtkVector3d currentPlaneJVector(thirdPlanePoint - firstPlanePoint);
    vtkVector3d currentPlaneKVector(currentPlaneIVector.Cross(currentPlaneJVector));

    // Attempt to compute plane equation with brute force
    // if the heuristic choice of plane points produces collinear vectors
    if (currentPlaneKVector.GetX() == 0 && currentPlaneKVector.GetY() == 0 && currentPlaneKVector.GetZ() == 0)
    {
        for (int pointIndex = 0; pointIndex<numberOfPoints - 2; ++pointIndex)
        {
            contourPoints->GetPoint(pointIndex, firstPointArray);
            contourPoints->GetPoint(pointIndex + 1, secondPointArray);
            contourPoints->GetPoint(pointIndex + 2, thirdPointArray);
            firstPlanePoint = vtkVector3d(firstPointArray[0], firstPointArray[1], firstPointArray[2]);
            secondPlanePoint = vtkVector3d(secondPointArray[0], secondPointArray[1], secondPointArray[2]);
            thirdPlanePoint = vtkVector3d(thirdPointArray[0], thirdPointArray[1], thirdPointArray[2]);
            currentPlaneIVector = secondPlanePoint - firstPlanePoint;
            currentPlaneJVector = thirdPlanePoint - firstPlanePoint;
            currentPlaneKVector = currentPlaneIVector.Cross(currentPlaneJVector);

            if (!(currentPlaneKVector.GetX() == 0 && currentPlaneKVector.GetY() == 0 && currentPlaneKVector.GetZ() == 0))
            {
                break;
            }
        }

        // Check for valid plane vector and report failure if all attempts failed to produce a valid one
        if (currentPlaneKVector.GetX() == 0 && currentPlaneKVector.GetY() == 0 && currentPlaneKVector.GetZ() == 0)
        {
            return false;
        }
    }

    // Normalize normal vector
    currentPlaneKVector.Normalize();

    // Setup output computed plane
    contourPlane->SetNormal(currentPlaneKVector.GetX(), currentPlaneKVector.GetY(), currentPlaneKVector.GetZ());
    contourPlane->SetOrigin(firstPlanePoint.GetX(), firstPlanePoint.GetY(), firstPlanePoint.GetZ());

    return true;
}

//----------------------------------------------------------------------------
double vtkPlanarContourToRibbonModelConversionRule::ComputeContourPlaneSpacing(vtkPolyData* planarContourPolyData, vtkPlane* contoursPlane)
{
    // Check input planar contour for suitable number of planes
    int numberOfPlanes = planarContourPolyData->GetNumberOfCells();
    if (!numberOfPlanes)
    {
        return 1.0;
    }
    if (numberOfPlanes < 2)
    {
        return 1.0;
    }

    // Get all contour points
    vtkPoints* points = planarContourPolyData->GetPoints();

    // Computed output variables
    std::vector<double> planeSpacingValues;
    bool consistentPlaneSpacing = true;
    double distanceBetweenContourPlanes = -1.0; // The found distance between planes if consistent

                                                // All contour planes, with their distances from the first plane
    std::map< vtkSmartPointer<vtkPlane>, double > contourPlanes;
    double firstNormal[3] = { 0.0,0.0,0.0 };
    double firstOrigin[3] = { 0.0,0.0,0.0 };

    // Iterate over each contour in the set, computing planar spacing values for every contour
    for (int contourIndex = 0; contourIndex < planarContourPolyData->GetNumberOfCells(); ++contourIndex)
    {
        // Get contour cell
        vtkCell* currentContour = planarContourPolyData->GetCell(contourIndex);

        // Compute contour plane
        vtkSmartPointer<vtkPlane> currentContourPlane = vtkSmartPointer<vtkPlane>::New();
        bool validPlane = ComputePlaneForContour(points, currentContour, currentContourPlane);
        if (!validPlane)
        {
            continue;
        }

        // Store first plane parameters to compute distances
        if (contourPlanes.empty())
        {
            currentContourPlane->GetNormal(firstNormal);
            currentContourPlane->GetOrigin(firstOrigin);
            contourPlanes[currentContourPlane] = 0.0;

            // Set first valid plane as output contours plane
            if (contoursPlane)
            {
                contoursPlane->SetOrigin(currentContourPlane->GetOrigin());
                contoursPlane->SetNormal(currentContourPlane->GetNormal());
            }
        }
        // Compute distance from first plane (also check if they are parallel)
        else
        {
            double normal[3] = { 0.0,0.0,0.0 };
            currentContourPlane->GetNormal(normal);
            // We accept normal vectors with the exact opposite direction, so we accept if their dot product is 1 or -1 (normal vectors have magnitude of 1)
            double dotProduct = (normal[0] * firstNormal[0]) + (normal[1] * firstNormal[1]) + (normal[2] * firstNormal[2]);
            if (!AreEqualWithTolerance(fabs(dotProduct), 1.0))
            {
            }

            // Store distance of current plane from first plane
            double distanceFromFirstPlane = vtkPlane::DistanceToPlane(firstOrigin, normal, currentContourPlane->GetOrigin());
            contourPlanes[currentContourPlane] = distanceFromFirstPlane;
        }
    } // For all contour planes

      // Order computed contour planes
    std::map< double, vtkSmartPointer<vtkPlane> > orderedContourPlanes;
    std::map< vtkSmartPointer<vtkPlane>, double >::iterator planesIt;
    for (planesIt = contourPlanes.begin(); planesIt != contourPlanes.end(); ++planesIt)
    {
        // Swap map to have the ordering by distance from first plane
        orderedContourPlanes[planesIt->second] = planesIt->first;
    }

    // Compute distances between adjacent planes
    std::map< double, vtkSmartPointer<vtkPlane> >::iterator orderedPlanesIt;
    double previousDistance = 0.0;
    for (orderedPlanesIt = orderedContourPlanes.begin(); orderedPlanesIt != orderedContourPlanes.end(); ++orderedPlanesIt)
    {
        if (orderedPlanesIt != orderedContourPlanes.begin()) // We skip the first one, just save its distance as previous
        {
            double currentDistance = fabs(orderedPlanesIt->first - previousDistance);
            if (!AreEqualWithTolerance(currentDistance, 0.0))
            {
                // Only add spacing value if it's not 0 - multiple contours may be drawn on the same plane and it's not considered for slice thickness computation
                planeSpacingValues.push_back(currentDistance);

                // Store current spacing as found spacing if has not been set
                if (distanceBetweenContourPlanes == -1.0)
                {
                    distanceBetweenContourPlanes = currentDistance;
                }

                // Check for inconsistency
                if (!AreEqualWithTolerance(currentDistance, distanceBetweenContourPlanes)
                    && consistentPlaneSpacing) // Only prompt the warning once
                {
                    consistentPlaneSpacing = false;
                }
            } // If non-zero
        }
        previousDistance = orderedPlanesIt->first;
    }

    // Calculate the majority value for the plane spacing from plane spacing values if inconsistent spacing was found
    if (!consistentPlaneSpacing)
    {
        std::string message("");
        distanceBetweenContourPlanes = MajorityValue(planeSpacingValues, message);
    }

    return distanceBetweenContourPlanes;
}
