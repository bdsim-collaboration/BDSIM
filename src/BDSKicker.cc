#include "BDSGlobalConstants.hh" 
#include "BDSKicker.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSSbendMagField.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

BDSKicker::BDSKicker(G4String aName, G4double aLength, 
		     G4double bpRad, G4double FeRad,
		     G4double bField, G4double angle, G4double outR,
		     G4double tilt, G4double bGrad, 
		     G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, aTunnelMaterial, aMaterial,
	       0, 0, /*itsAngle=*/0)
{
  SetOuterRadius(outR);
  itsTilt      = tilt;
  itsBField    = bField;
  itsBGrad     = bGrad;
  //use separate kick angle as kicker doesn't rotate reference trajectory
  //bdsbeamline places things based on itsAngle
  itsKickAngle = angle;
}

BDSKicker::BDSKicker(G4String     name,
		     G4double     length,
		     G4double     bField,
		     G4double     bGrad,
		     G4double     angle,
		     G4bool       verticalKicker,
		     beamPipeInfo beamPipeInfo,
		     G4double     boxSize,
		     G4String     outerMaterial,
		     G4String     tunnelMaterial,
		     G4double     tunnelRadius,
		     G4double     tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfo,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
  itsBField(bField),itsBGrad(bGrad),itsKickAngle(angle),isVerticalKicker(verticalKicker)
{;}

void BDSKicker::Build()
{
  BDSMultipole::Build();
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
      kickerAper1 = aper2; //vertical is rotated in the end during placement
      kickerAper2 = aper1; //so build aperture otherway - sway 1,2 - x,y
    }
  else
    {
      kickerAper1 = aper1;
      kickerAper2 = aper2;
    }
  
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeType,
							    itsName,
							    itsLength,
							    kickerAper1,
							    kickerAper2,
							    aper3,
							    aper4,
							    vacuumMaterial,
							    beamPipeThickness,
							    beamPipeMaterial);

  //manully do BeamPipeCommonTasks here as rotation in placement can be different

  // SET FIELD
  beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);

  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  // register logical volumes using geometry component base class
  RegisterLogicalVolumes(beampipe->GetAllLogicalVolumes());

  // if it's a vertical kicker, rotate the beam pipe by 90 degrees
  // this also rotates the dipole stepper in the vacuum volume
  G4RotationMatrix* kickerRotation = new G4RotationMatrix();
  if (isVerticalKicker)
    {kickerRotation->rotateZ(CLHEP::pi*0.5);}

  // place beampipe
  itsPhysiComp = new G4PVPlacement(kickerRotation,            // rotation
				   (G4ThreeVector)0,          // at (0,0,0)
				   beampipe->GetContainerLogicalVolume(),  // its logical volume
				   itsName+"_bmp_phys",	      // its name
				   itsMarkerLogicalVolume,    // its mother  volume
				   false,                     // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

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

void BDSKicker::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,0,1));
  itsVisAttributes->SetForceSolid(true);
}


void BDSKicker::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.); // note the - sign...
  itsMagField=new BDSSbendMagField(Bfield,itsLength,itsKickAngle);
  
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  dipoleStepper->SetBField(-itsBField); // note the - sign...
  dipoleStepper->SetBGrad(itsBGrad);
  itsStepper = dipoleStepper;
}

void BDSKicker::BuildOuterVolume()
{
  return;
}
