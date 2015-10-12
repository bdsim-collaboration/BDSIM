#include "BDSGlobalConstants.hh" 
#include "BDSKicker.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

BDSKicker::BDSKicker(G4String            name,
		     G4double            length,
		     G4double            bFieldIn,
		     G4double            bGradIn,
		     G4double            kickAngle,
		     G4bool              verticalKickerIn,
		     BDSBeamPipeInfo*    beamPipeInfo,
		     BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::hkicker, name, length,
	    beamPipeInfo, magnetOuterInfo),
  bField(bFieldIn),
  bGrad(bGradIn),
  kickAngle(kickAngle),
  verticalKicker(verticalKickerIn)
{
  if (verticalKicker)
    {magnetType = BDSMagnetType::vkicker;}
}

void BDSKicker::Build()
{
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=0.;
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius();
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	 BDSGlobalConstants::Instance()->GetMagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(2, BFldIron,0);
    }
}

void BDSKicker::BuildBeampipe()
{
  // have to distinguish if it's a vertical or horizontal kicker
  // but without rotating the aperture model - ie input parameters aper1
  // still represents horizontal

  G4double kickerAper1, kickerAper2;
  if (verticalKicker)
    {
      kickerAper1 = beamPipeInfo->aper2; //vertical is rotated in the end during placement
      kickerAper2 = beamPipeInfo->aper1; //so build aperture otherway - sway 1,2 - x,y
    }
  else
    {
      kickerAper1 = beamPipeInfo->aper1;
      kickerAper2 = beamPipeInfo->aper2;
    }

  BDSBeamPipeFactory* fac = BDSBeamPipeFactory::Instance();
  beampipe = fac->CreateBeamPipe(beamPipeInfo->beamPipeType,
				 name,
				 chordLength,
				 kickerAper1,
				 kickerAper2,
				 beamPipeInfo->aper3,
				 beamPipeInfo->aper4,
				 beamPipeInfo->vacuumMaterial,
				 beamPipeInfo->beamPipeThickness,
				 beamPipeInfo->beamPipeMaterial);

  //manually do BeamPipeCommonTasks here as rotation in placement can be different

  // SET FIELD
  beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);

  // if it's a vertical kicker, rotate the beam pipe by 90 degrees
  // this also rotates the dipole stepper in the vacuum volume
  G4RotationMatrix* kickerRotation = new G4RotationMatrix();
  if (verticalKicker)
    {kickerRotation->rotateZ(CLHEP::pi*0.5);}
  RegisterRotationMatrix(kickerRotation);

  // place beampipe
  G4PVPlacement* pipePV = new G4PVPlacement(kickerRotation,                        // rotation
					    (G4ThreeVector)0,                      // at (0,0,0)
					    beampipe->GetContainerLogicalVolume(), // its logical volume
					    name + "_beampipe_pv",	          // its name
					    containerLogicalVolume,                // its mother  volume
					    false,                                 // no boolean operation
					    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

  RegisterPhysicalVolume(pipePV);
  
  // record extent of geometry
  if (verticalKicker)
    {
      SetExtentX(beampipe->GetExtentY());
      SetExtentY(beampipe->GetExtentX());
    }
  else
    {
      SetExtentX(beampipe->GetExtentX());
      SetExtentY(beampipe->GetExtentY());
    }
  SetExtentZ(beampipe->GetExtentZ());
} 

void BDSKicker::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector vectorBField(0.,bField,0.);
  itsMagField = new BDSSbendMagField(vectorBField,chordLength,kickAngle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  BDSDipoleStepper* stepper = new BDSDipoleStepper(itsEqRhs);
  stepper->SetBField(bField);
  stepper->SetBGrad(bGrad);
  itsStepper = stepper; // assigned to base class pointer
}
