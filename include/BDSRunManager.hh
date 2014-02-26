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
#include "globals.hh"
#include  <vector>
#include "BDSWorld.hh"

class BDSRunManager:public G4RunManager
{
  public: // with description

    static BDSRunManager* GetRunManager();
    //  Static method which returns the singleton pointer of BDSRunManager or
    // its derived class.

  private:
  static BDSRunManager* fRunManager;
  int GetNumberOfMesh();

  public: // with description
    BDSRunManager();
    virtual ~BDSRunManager();
    //  The constructor and the destructor. The user must construct this class
    // object at the beginning of his/her main() and must delete it at the 
    // bottom of the main().

  public: // with description
//SPM
  virtual void DoEventLoop(G4int n_event,const char* macroFile,G4int n_select);
  virtual void BeamOn(G4int n_event,const char* macroFile=0,G4int n_select=-1);
  inline void RunInit(){
    RunInitialization();
  }
};
#endif

