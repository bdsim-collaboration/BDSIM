#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSKicker.hh"

#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSUtilities.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

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
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << type.ToString() << ", angle: " << kickAngle << G4endl;
#endif
}

void BDSKicker::Build()
{
  BDSMagnet::Build();
  /*
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
  */
}
