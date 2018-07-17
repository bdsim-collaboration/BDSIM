/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "globals.hh" // geant4 globals / types

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMaterials.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUndulator.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "G4Transform3D.hh"

using namespace GMAD;

BDSUndulator::BDSUndulator (G4String   nameIn,
                            G4double   lengthIn,
                            G4double   outerDiameterIn,
                            G4double   periodIn,
                            G4double   magnetHeightIn,
                            G4double   undulatorGapIn,
                            G4String   materialIn ):
        BDSAcceleratorComponent(nameIn, lengthIn, 0, "undulator"),
        outerDiameter(outerDiameterIn),
        undulatorPeriod(periodIn),
        material(materialIn),
        magnetHeight(magnetHeightIn),
        magnetGap(undulatorGapIn)
{;}

BDSUndulator::~BDSUndulator()
{;}

void BDSUndulator::BuildContainerLogicalVolume()
{
  //Input Checks
  if (outerDiameter <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"outerDiameter\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  if (BDS::IsFinite(fmod(arcLength, undulatorPeriod)))
    {
      G4cerr << __METHOD_NAME__ << "Undulator length \"arcLength\" does not divide into an integer number of "
            "undulator periods (length \"undulatorPeriod\"" <<  G4endl;
      exit(1);
    }
  if (!BDS::IsFinite(undulatorPeriod))
    {
      G4cerr << __METHOD_NAME__ << "Undulator period is 0, period must be finite" <<  G4endl;
      exit(1);
    }

  containerSolid = new G4Box(name + "_container_solid",
                             outerDiameter*0.5,
                             outerDiameter*0.5,
                             chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");
}

void BDSUndulator::Build()
{
  BDSAcceleratorComponent::Build();
}

void BDSUndulator::BuildUndulatorMagnet()
{;}