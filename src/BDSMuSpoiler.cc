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
#include "BDSMuSpoilerMagField.hh"

class BDSTiltOffset;

BDSMuSpoiler::BDSMuSpoiler(G4String            name,
			   G4double            length,
			   G4double            bField,
			   BDSBeamPipeInfo*    beamPipeInfo,
			   BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::muspoiler, name, length,
	    beamPipeInfo, magnetOuterInfo),
  itsBField(bField)
{;}

void BDSMuSpoiler::BuildBPFieldAndStepper()
{
  // don't do anything and leave as null pointers
  // don't worry, they won't be attached if null - whew
  return;
}

void BDSMuSpoiler::BuildOuterFieldManager(G4int    /*nPoles*/,
					  G4double /*poleField*/,
					  G4double /*phiOffset*/)
{
  // prepare and attach field
  itsOuterMagField = new BDSMuSpoilerMagField(itsBField);
  itsOuterFieldMgr = new G4FieldManager(itsOuterMagField);

  // these options are always non-zero so always set them
  itsOuterFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->DeltaIntersection());
  itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());

  // NOTE this one was commented out - but no reason why - reinstated for v0.81
  itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->DeltaOneStep());
}

BDSMuSpoiler::~BDSMuSpoiler()
{;}
