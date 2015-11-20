#include "BDSGlobalConstants.hh" 
#include "BDSKicker.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"

#include "G4FieldManager.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"


BDSKicker::BDSKicker(G4String            name,
		     G4double            length,
		     G4double            bFieldIn,
		     G4double            kickAngle,
		     BDSMagnetType       type,
		     BDSBeamPipeInfo*    beamPipeInfo,
		     BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(type, name, length,
	    beamPipeInfo, magnetOuterInfo),
  bField(bFieldIn),
  kickAngle(kickAngle)
{;}

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
  BDSBeamPipeFactory* fac = BDSBeamPipeFactory::Instance();
  beampipe = fac->CreateBeamPipe(name,
				 chordLength - lengthSafety,
				 beamPipeInfo);
  
  RegisterDaughter(beampipe);

  // attach field to correct volume
  beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);

  // place beampipe
  G4PVPlacement* pipePV = new G4PVPlacement(nullptr,                               // rotation
					    (G4ThreeVector)0,                      // at (0,0,0)
					    beampipe->GetContainerLogicalVolume(), // its logical volume
					    name + "_beampipe_pv",	           // its name
					    containerLogicalVolume,                // its mother  volume
					    false,                                 // no boolean operation
					    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

  RegisterPhysicalVolume(pipePV);
  
  // record extent of geometry
  InheritExtents(beampipe);
} 

void BDSKicker::BuildBPFieldAndStepper()
{
  // don't build field if angle is 0 as stepper intolerant of this and wast of memory and cpu
  if (BDS::IsFinite(kickAngle))
    {
      // set up the magnetic field and stepper
      // set magnetic field direction dependingon whether it's a vertical kicker or not
      G4ThreeVector vectorBField;
      if (type == BDSMagnetType::vkicker)
	{vectorBField = G4ThreeVector(-bField, 0, 0);}
      else
	{vectorBField = G4ThreeVector(0, bField, 0);}
      
      itsMagField = new BDSSbendMagField(vectorBField,chordLength,kickAngle);
      itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
      /*
	BDSDipoleStepper* stepper = new BDSDipoleStepper(itsEqRhs);
	stepper->SetBField(bField);
	stepper->SetBGrad(bGrad);
	itsStepper = stepper; // assigned to base class pointer
      */
      
      // Use general G4 stepper for uniform field as field is specified along
      // x for vkicker and not rotated and BDSDipoleStepper only works in the
      // horizontal plane.
      itsStepper = new G4HelixExplicitEuler(itsEqRhs);
    }
}
