#include "BDSGlobalConstants.hh" 

#include "G4FieldManager.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMuSpoiler.hh"
#include "BDSMuSpoilerMagField.hh"

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
  itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->MinimumEpsilonStep());
  itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->MaximumEpsilonStep());

  // NOTE this one was commented out - but no reason why - reinstated for v0.81
  itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->DeltaOneStep());
}

BDSMuSpoiler::~BDSMuSpoiler()
{;}
