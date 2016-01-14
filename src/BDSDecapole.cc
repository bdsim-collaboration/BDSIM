#include "BDSGlobalConstants.hh" 

#include "BDSDecapole.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

BDSDecapole::BDSDecapole(G4String            name,
			 G4double            length,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::decapole, name, length,
	    beamPipeInfo,magnetOuterInfo)
{;}

void BDSDecapole::Build()
{
  BDSMagnet::Build();
  /*
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/10);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/10);
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
      
      BuildOuterFieldManager(10, BFldIron,CLHEP::pi/10);
    }
  */
}
