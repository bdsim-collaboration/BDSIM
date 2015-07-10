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

BDSKicker::BDSKicker(G4String           name,
		     G4double           length,
		     G4double           bField,
		     G4double           bGrad,
		     G4double           angle,
		     G4bool             verticalKicker,
		     BDSBeamPipeInfo*   beamPipeInfo,
		     BDSMagnetOuterInfo magnetOuterInfo):
  BDSMagnet(BDSMagnetType::hkicker, name, length,
	    beamPipeInfo, magnetOuterInfo),
  itsBField(bField),
  itsBGrad(bGrad),
  itsKickAngle(angle),
  isVerticalKicker(verticalKicker)
{
  if (verticalKicker)
    {itsType = BDSMagnetType::vkicker;}
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
  if (isVerticalKicker)
    {
      kickerAper1 = beamPipeInfo->aper2; //vertical is rotated in the end during placement
      kickerAper2 = beamPipeInfo->aper1; //so build aperture otherway - sway 1,2 - x,y
    }
  else
    {
      kickerAper1 = beamPipeInfo->aper1;
      kickerAper2 = beamPipeInfo->aper2;
    }
  
  beampipe =
    BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeInfo->beamPipeType,
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

  // register objects using geometry component base class
  InheritObjects(beampipe);

  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
    {RegisterSensitiveVolume(beampipe->GetAllSensitiveVolumes());}

  // if it's a vertical kicker, rotate the beam pipe by 90 degrees
  // this also rotates the dipole stepper in the vacuum volume
  G4RotationMatrix* kickerRotation = new G4RotationMatrix();
  if (isVerticalKicker)
    {kickerRotation->rotateZ(CLHEP::pi*0.5);}
  RegisterRotationMatrix(kickerRotation);

  // place beampipe
  itsPhysiComp = new G4PVPlacement(kickerRotation,                        // rotation
				   (G4ThreeVector)0,                      // at (0,0,0)
				   beampipe->GetContainerLogicalVolume(), // its logical volume
				   name + "_beampipe_pv",	          // its name
				   containerLogicalVolume,                // its mother  volume
				   false,                                 // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

  RegisterPhysicalVolume(itsPhysiComp);
  
  // record extent of geometry
  if (isVerticalKicker){
    SetExtentX(beampipe->GetExtentY());
    SetExtentY(beampipe->GetExtentX());
  }
  else {
    SetExtentX(beampipe->GetExtentX());
    SetExtentY(beampipe->GetExtentY());
  }
  SetExtentZ(beampipe->GetExtentZ());
} 

void BDSKicker::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.); // note the - sign...
  itsMagField = new BDSSbendMagField(Bfield,chordLength,itsKickAngle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  BDSDipoleStepper* stepper = new BDSDipoleStepper(itsEqRhs);
  stepper->SetBField(-itsBField); // note the - sign...
  stepper->SetBGrad(itsBGrad);
  itsStepper = stepper; // assigned to base class pointer
}
