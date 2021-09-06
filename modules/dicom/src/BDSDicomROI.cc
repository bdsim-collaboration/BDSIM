/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2021.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "BDSDicomROI.hh"

BDSDicomROI::BDSDicomROI(int ROINumber, OFString ROIName) : theNumber(ROINumber + 1), theName(ROIName)
{
    G4cout << " CREATED  DicomROI " << theNumber << " : " << theName << G4endl; //GDEB
}

void BDSDicomROI::AddContour(BDSDicomROIContour *cont)
{
    // check if it already exists a contour with same Z
    G4bool bOK = false;
    size_t ii;
    for (ii = 0; ii < theContours.size(); ii++)
    {
        if (cont->GetZ() == theContours[ii]->GetZ())
        {
            bOK = true;
            break;
        }
    }
    if (!bOK)
    {
        theContours.push_back(cont);
    }
    else
    {
        theContours[ii]->AddPoints(cont->GetPoints());
        delete cont;
    }
}

void BDSDicomROI::Print(std::ostream &out)
{
    out << "@@@@@ ROI: " << theNumber << " " << theName << G4endl;

    out << "@@@@ NUMBER OF ContourSequences " << theContours.size() << G4endl;
    for (size_t ii = 0; ii < theContours.size(); ii++)
    {
        out << "@@@@ CONTOUR " << ii << G4endl;
        theContours[ii]->Print(out);
    }
}
