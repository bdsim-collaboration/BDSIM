#ifndef BDSRUNMANAGER_H
#define BDSRUNMANAGER_H

#include "G4RunManager.hh"

#include "globals.hh"

/**
 * @brief Wrapper from G4RunManager that provides more output.
 *
 */

class BDSRunManager: public G4RunManager
{
public:
  BDSRunManager();

  virtual ~BDSRunManager();

  /// For additional output
  virtual void DoEventLoop(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  
  /// Altered BeamOn function to account for Placet synchronisation
  virtual void BeamOn(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  
  /// For additional output
  virtual void ProcessOneEvent(G4int i_event);

};
#endif

