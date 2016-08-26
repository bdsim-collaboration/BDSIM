#include "BDSBeamPipeInfo.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSOctupole.hh"
#include "BDSOctMagField.hh"
#include "BDSOctStepper.hh"

#include "G4Mag_UsualEqRhs.hh"

BDSOctupole::BDSOctupole(G4String            name,
			 G4double            length,
			 G4double            bTriplePrime,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::octupole, name, length,
	    beamPipeInfo, magnetOuterInfo),
  itsBTriplePrime(bTriplePrime)
{;}

void BDSOctupole::Build()
{
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->MagnetPoleRadius()*sin(CLHEP::pi/8);
      polePos[1]= BDSGlobalConstants::Instance()->MagnetPoleRadius()*cos(CLHEP::pi/8);
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
            BDSGlobalConstants::Instance()->MagnetPoleSize()/
	(BDSGlobalConstants::Instance()->ComponentBoxSize()/2-
            BDSGlobalConstants::Instance()->MagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(8, BFldIron,CLHEP::pi/8);
    }
}

void BDSOctupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSOctMagField(itsBTriplePrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  BDSOctStepper* octStepper=new BDSOctStepper(itsEqRhs);
  octStepper->SetBTrpPrime(itsBTriplePrime);
  itsStepper = octStepper;
}
