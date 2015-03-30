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
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMultipole.hh"
#include "BDSMuSpoiler.hh"
#include "BDSMuSpoilerMagField.hh"

BDSMuSpoiler::BDSMuSpoiler(G4String        name,
			   G4double        length,
			   G4double        bField,
			   BDSBeamPipeInfo beamPipeInfoIn,
			   G4double        boxSize,
			   G4String        outerMaterial,
			   G4String        tunnelMaterial,
			   G4double        tunnelRadius,
			   G4double        tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
  itsBField(bField)
  // outerMagField(NULL),
  // outerFieldMgr(NULL)
{;}

void BDSMuSpoiler::BuildBPFieldAndStepper()
{
  // don't do anything and leave as null pointers
  // don't worry, they won't be attached if null - whew
  return;
}

void BDSMuSpoiler::BuildOuterLogicalVolume(bool /*outerMaterialIsVacuum*/)
{
  //whole point is the outerlogical volume so ignore the outerMaterialIsVacuum flag
  
  //void BDSMuSpoiler::BuildMuSpoiler()
  //{

  BDSMultipole::BuildOuterLogicalVolume(false);

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
  //  {itsItsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());}
  itsOuterLogicalVolume->SetFieldManager(itsOuterFieldMgr,false);

}

void BDSMuSpoiler::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.0,0.5,0.5));
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
}

BDSMuSpoiler::~BDSMuSpoiler()
{
  // delete outerMagField;
  // delete outerFieldMgr;
}
