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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSCrystalPosition.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

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
      crystalLeft = fac->CreateCrystal(name + "_left_", crystalInfoLeft);
      RegisterDaughter(crystalLeft);
    }
  if (crystalInfoRight)
    {
      crystalRight = fac->CreateCrystal(name + "_right_", crystalInfoRight);
      RegisterDaughter(crystalRight);
    }
  delete fac;

  if (crystalLeft)
    {
      G4ThreeVector objectOffset     = crystalLeft->GetPlacementOffset();
      G4ThreeVector colOffsetL       = G4ThreeVector(halfGapLeft,0,0);
      G4ThreeVector placementOffset  = objectOffset + colOffsetL;
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisLeft))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  placementRot->rotate(angleYAxisLeft, G4ThreeVector(0,1,0)); // rotate about local unitY
	}

      // check if it'll fit..
      BDSExtent extShifted = (crystalLeft->GetExtent()).Translate(placementOffset);
      BDSExtent thisExtent = GetExtent();
      G4bool safe = thisExtent.Encompasses(extShifted);
      if (!safe)
	{G4cout << __METHOD_NAME__ << "Left crystal potential overlap" << G4endl;}
      
      auto cL = new G4PVPlacement(placementRot,
				  placementOffset,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_left_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  true); // always check
      RegisterPhysicalVolume(cL);
    }
  if (crystalRight)
    {
      G4ThreeVector objectOffset     = crystalLeft->GetPlacementOffset();
      G4ThreeVector colOffsetR       = G4ThreeVector(-halfGapRight,0,0); // -ve as r.h. coord system
      G4ThreeVector placementOffset  = objectOffset + colOffsetR;
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisRight))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  placementRot->rotate(angleYAxisRight, G4ThreeVector(0,1,0)); // rotate about local unitY
	}
      
      // check if it'll fit..
      BDSExtent extShifted = (crystalRight->GetExtent()).Translate(placementOffset);
      BDSExtent thisExtent = GetExtent();
      G4bool safe = thisExtent.Encompasses(extShifted);
      if (!safe)
	{G4cout << __METHOD_NAME__ << "Right crystal potential overlap" << G4endl;}
      
      auto cR = new G4PVPlacement(placementRot,
				  placementOffset,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_right_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  true); // always check
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
