/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 9.3.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSPlanckScatterBuilder.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"

#include "BDSPlanckScatter.hh"

BDSPlanckScatterBuilder::BDSPlanckScatterBuilder() 
{}

BDSPlanckScatterBuilder::
~BDSPlanckScatterBuilder() {}


void BDSPlanckScatterBuilder::Build()
{

  G4ProcessManager * pManager = nullptr;
  
  pManager = G4Electron::Electron()->GetProcessManager();
  pManager->AddProcess(&thePlanckScatterProcess);
  pManager->SetProcessOrderingToLast(&thePlanckScatterProcess,idxPostStep);

  pManager = G4Positron::Positron()->GetProcessManager();
  pManager->AddProcess(&thePlanckScatterProcess);
  pManager->SetProcessOrderingToLast(&thePlanckScatterProcess,idxPostStep);

}
// 2002 by J.P. Wellisch
