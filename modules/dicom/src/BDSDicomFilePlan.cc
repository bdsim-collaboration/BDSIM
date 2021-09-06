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

#include "BDSDicomFilePlan.hh"

#include "G4ThreeVector.hh"

#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/seq/drtfgs.h"   // DRTFractionGroupSequence
#include "dcmtk/dcmrt/seq/drtrbs8.h"  // DRTReferencedBeamSequenceInRTFractionSchemeModule
#include "dcmtk/dcmrt/seq/drtbs.h"    // for BeamSequence
#include "dcmtk/dcmrt/seq/drtblds1.h" // for BeamLimitingDeviceSequence
#include "dcmtk/dcmrt/seq/drtcps.h"   // for ControlPointSequence
#include "dcmtk/dcmrt/seq/drtbldps.h" // for BeamLimitingDevicePositionSequence
#include "dcmtk/dcmrt/seq/drtcos.h"   // for CompensatorSequence
#include "dcmtk/dcmrt/seq/drtbl2.h"   // for BlockSequence
#include "dcmtk/dcmrt/seq/drtws.h"    // for WedgeSequence

BDSDicomFilePlan::BDSDicomFilePlan(DcmDataset *dset) : BDSDicomVFile(dset)
{
    ;
}

void BDSDicomFilePlan::ReadData()
{
    DRTPlanIOD rtplan;
    OFCondition result = rtplan.read(*theDataset);
    if (!result.good())
    {
        G4Exception("DicomFilePlan::ReadData",
                    "DFS001",
                    FatalException,
                    result.text());
    }
    OFString fstr;
    Sint32 fint;
    OFVector<Float64> fvfloat;

    DRTFractionGroupSequence frgSeq = rtplan.getFractionGroupSequence();
    if (frgSeq.isEmpty())
    {
        G4Exception("DicomFilePlan::ReadData",
                    "DFS002",
                    JustWarning,
                    "DRTFractionGroupSequence is empty");
    }
    G4cout << "@@@@@ NUMBER OF FractionSequences " << frgSeq.getNumberOfItems() << G4endl;
    frgSeq.gotoFirstItem();
    for (size_t i1 = 0; i1 < frgSeq.getNumberOfItems(); i1++)
    {
        DRTFractionGroupSequence::Item &rfgItem = frgSeq.getCurrentItem();
        rfgItem.getFractionGroupDescription(fstr);
        G4cout << " " << i1 << " FractionGroupDescription " << fstr << G4endl;
        rfgItem.getFractionGroupNumber(fint);
        G4cout << " " << i1 << " FractionGroupNumber " << fint << G4endl;
        rfgItem.getFractionPattern(fstr);
        G4cout << " " << i1 << " FractionPattern " << fstr << G4endl;
        rfgItem.getNumberOfBrachyApplicationSetups(fint);
        G4cout << " " << i1 << " NumberOfBrachyApplicationSetups " << fint << G4endl;
        CheckData0(" NumberOfBrachyApplicationSetups ", fint);
        rfgItem.getNumberOfFractionPatternDigitsPerDay(fint);
        G4cout << " " << i1 << " NumberOfFractionPatternDigitsPerDay " << fint << G4endl;
        rfgItem.getNumberOfFractionsPlanned(fint);
        G4cout << " " << i1 << " NumberOfFractionsPlanned " << fint << G4endl;
        rfgItem.getRepeatFractionCycleLength(fint);
        G4cout << " " << i1 << " RepeatFractionCycleLength " << fint << G4endl;
        frgSeq.gotoNextItem();
    }
}

void BDSDicomFilePlan::CheckData0(OFString title, Sint32 val)
{
    if (val != 0)
    {
        G4Exception("DicomFilePlan::CheckData",
                    "DFP003",
                    FatalException,
                    (title + " exists, and code is not ready ").c_str());
    }
}