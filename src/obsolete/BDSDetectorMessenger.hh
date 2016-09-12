/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSDetectorMessenger.hh,v 1.1 2005/01/22 17:05:30 agapov Exp $
// GEANT4 tag $Name:  $
//
// 

//===================================================.
//===================================================.

#ifndef BDSDetectorMessenger_h
#define BDSDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class BDSDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//===================================================.

class BDSDetectorMessenger: public G4UImessenger
{
public:
  BDSDetectorMessenger(BDSDetectorConstruction* );
  ~BDSDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  BDSDetectorConstruction* BDSDetector;    

  //    G4UIdirectory*             BDSdetDir;
  //    G4UIcmdWithAString*        AbsMaterCmd;
  //    G4UIcmdWithAString*        GapMaterCmd;
  //    G4UIcmdWithADoubleAndUnit* AbsThickCmd;
  //    G4UIcmdWithADoubleAndUnit* GapThickCmd;
  //    G4UIcmdWithADoubleAndUnit* SizeYZCmd;
  //    G4UIcmdWithAnInteger*      NbLayersCmd;    
  //    G4UIcmdWithADoubleAndUnit* MagFieldCmd;
  //    G4UIcmdWithoutParameter*   UpdateCmd;
};

#endif

