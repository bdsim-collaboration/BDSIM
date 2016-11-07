#ifndef BDSPLANCKSCATTERBUILDER_H
#define BDSPLANCKSCATTERBUILDER_H

#include "globals.hh"
#include "G4ios.hh"

#include "BDSPlanckScatter.hh"
#include "G4VProcess.hh"

class BDSPlanckScatterBuilder 
{
public: 
  BDSPlanckScatterBuilder();
  virtual ~BDSPlanckScatterBuilder();
  
public: 
  void Build();
  
protected:
  BDSPlanckScatter thePlanckScatterProcess;
};
// 2003 by G.A Blair (after J.P. Wellisch)
#endif





