/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 12.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

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

BDSMuSpoiler::BDSMuSpoiler(G4String           name,
			   G4double           length,
			   G4double           bField,
			   BDSBeamPipeInfo*   beamPipeInfo,
			   BDSMagnetOuterInfo magnetOuterInfo):
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

void BDSMuSpoiler::BuildOuterVolume()
{
  BDSMagnet::BuildOuterVolume();

  // prepare and attach field
  delete itsOuterMagField;
  delete itsOuterFieldMgr;
  itsOuterMagField = new BDSMuSpoilerMagField(itsBField);
  itsOuterFieldMgr = new G4FieldManager(itsOuterMagField);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0)
    {itsOuterFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());}
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    {itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());}
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    {itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());}
  //if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
  //  {itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());}
  if(outer)
    {outer->GetContainerLogicalVolume()->SetFieldManager(itsOuterFieldMgr,false);}
}

BDSMuSpoiler::~BDSMuSpoiler()
{;}
