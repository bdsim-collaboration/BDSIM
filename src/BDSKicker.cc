#include "BDSGlobalConstants.hh" 

#include "BDSKicker.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"

#include <map>

//============================================================

BDSKicker::BDSKicker(G4String aName, G4double aLength, 
		     G4double bpRad, G4double FeRad,
		     G4double bField, G4double angle, G4double outR,
		     G4double tilt, G4double bGrad, 
		     G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, aTunnelMaterial, aMaterial,
	       0, 0, angle)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsBField=bField;
  itsBGrad=bGrad;
  SetVisAttributes();
}

void BDSKicker::Build() {
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


void BDSKicker::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,0,1));
  itsVisAttributes->SetForceSolid(true);
}


void BDSKicker::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.); // note the - sign...
  itsMagField=new BDSSbendMagField(Bfield,itsLength,itsAngle);
  
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  itsStepper = new BDSDipoleStepper(itsEqRhs);
  BDSDipoleStepper* dipoleStepper = dynamic_cast<BDSDipoleStepper*>(itsStepper);
  dipoleStepper->SetBField(-itsBField); // note the - sign...
  dipoleStepper->SetBGrad(itsBGrad);
}

BDSKicker::~BDSKicker()
{
}
