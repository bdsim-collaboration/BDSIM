#include "BDSGlobalConstants.hh" 

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSMuSpoiler.hh"

class BDSTiltOffset;

BDSMuSpoiler::BDSMuSpoiler(G4String            name,
			   G4double            length,
			   G4double            /*bField*/,
			   BDSBeamPipeInfo*    beamPipeInfo,
			   BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::muonspoiler, name, length,
	    beamPipeInfo, magnetOuterInfo)
{;}
