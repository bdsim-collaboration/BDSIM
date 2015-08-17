/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

// $Id: BDSRunManager.hh,v 1.2 2007/07/18 13:35:01 malton Exp $
// GEANT4 tag $Name:  $
#ifndef BDSRunManager_h
#define BDSRunManager_h 1

#include "G4RunManager.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Version.hh"
#include "globals.hh"
#include  <vector>

class BDSRunManager:public G4RunManager
{
private:
  // add keyword ThreadLocal for versions 10
#if G4VERSION_NUMBER > 999
  G4ThreadLocal
#endif
  static BDSRunManager* fRunManager; // needed since singleton inheritance won't work

public:
  BDSRunManager();
  virtual ~BDSRunManager();
  static BDSRunManager* GetRunManager();
  
  // override virtual methods

  /// For additional output
  virtual void DoEventLoop(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  /// Altered BeamOn function to account for Placet synchronisation
  virtual void BeamOn(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  /// For additional output
  virtual void ProcessOneEvent(G4int i_event);

};
#endif

