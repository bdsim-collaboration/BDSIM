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
#include "BDSAcceleratorComponent.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSCrystalPosition.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

BDSCollimatorCrystal::BDSCollimatorCrystal(G4String           nameIn, 
					   G4double           lengthIn,
					   BDSBeamPipeInfo*   beamPipeInfoIn,
					   BDSCrystalPosition crystalPositionIn,
					   BDSCrystalInfo*    crystalInfoLeftIn,
					   BDSCrystalInfo*    crystalInfoRightIn,
					   const G4double&    halfGapLeftIn,
					   const G4double&    halfGapRightIn,
					   const G4double&    angleYAxisLeftIn,
					   const G4double&    angleYAxisRightIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "drift", beamPipeInfoIn),
  crystalPosition(crystalPositionIn),
  crystalInfoLeft(crystalInfoLeftIn),
  crystalInfoRight(crystalInfoRightIn),
  halfGapLeft(halfGapLeftIn),
  halfGapRight(halfGapRightIn),
  angleYAxisLeft(angleYAxisLeftIn),
  angleYAxisRight(angleYAxisRightIn)
{;}

BDSCollimatorCrystal::~BDSCollimatorCrystal()
{
  delete crystalInfoLeft;
  delete crystalInfoRight;
}

void BDSCollimatorCrystal::Build()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
					      chordLength,
					      beamPipeInfo);

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());

  BDSCrystalFactory* fac = new BDSCrystalFactory();
  if (crystalInfoLeft)
    {
      crystalLeft = fac->CreateCrystal(name, crystalInfoLeft);
      RegisterDaughter(crystalLeft);
    }
  if (crystalInfoRight)
    {
      crystalRight = fac->CreateCrystal(name, crystalInfoRight);
      RegisterDaughter(crystalRight);
    }
  delete fac;

  if (crystalLeft)
    {
      G4ThreeVector placementOffset  = crystalLeft->GetPlacementOffset();
      G4ThreeVector colOffsetL       = G4ThreeVector(halfGapLeft,0,0);
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      auto cL = new G4PVPlacement(placementRot,
				  placementOffset + colOffsetL,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_left_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  checkOverlaps);
      RegisterPhysicalVolume(cL);
    }
  if (crystalRight)
    {
      G4ThreeVector placementOffset  = crystalLeft->GetPlacementOffset();
      G4ThreeVector colOffsetR       = G4ThreeVector(-halfGapRight,0,0); // -ve as r.h. coord system
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      auto cR = new G4PVPlacement(placementRot,
				  placementOffset + colOffsetR,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_right_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  checkOverlaps);
      RegisterPhysicalVolume(cR);
    }
}

G4String BDSCollimatorCrystal::Material() const
{
  auto bpMat = GetBeamPipeInfo()->beamPipeMaterial;
  if (bpMat)
    {return bpMat->GetName();}
  else
    {return BDSAcceleratorComponent::Material();} // none
}
