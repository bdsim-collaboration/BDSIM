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
// $Id: BDSDetectorMessenger.cc,v 1.1 2005/01/22 16:42:31 agapov Exp $
// GEANT4 tag $Name:  $
//
// 

//=================================================================
//=================================================================

#include "BDSDetectorMessenger.hh"

#include "BDSDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//=================================================================

BDSDetectorMessenger::BDSDetectorMessenger(BDSDetectorConstruction * BDSDet)
:BDSDetector(BDSDet)
{ 
  /*
  BDSdetDir = new G4UIdirectory("/quad/");
  BDSdetDir->SetGuidance("BDS detector control.");
  */  
  //  AbsMaterCmd = new G4UIcmdWithAString("/quad/setAbsMat",this);
  // AbsMaterCmd->SetGuidance("Select Material of the Absorber.");
  //AbsMaterCmd->SetParameterName("choice",false);
  //AbsMaterCmd->AvailableForStates(Idle);
  
  //  GapMaterCmd = new G4UIcmdWithAString("/quad/setGapMat",this);
  //  GapMaterCmd->SetGuidance("Select Material of the Gap.");
  //  GapMaterCmd->SetParameterName("choice",false);
  //  GapMaterCmd->AvailableForStates(Idle);
    
  //  AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/quad/setAbsThick",this);
  //  AbsThickCmd->SetGuidance("Set Thickness of the Absorber");
  //  AbsThickCmd->SetParameterName("Size",false);
  //  AbsThickCmd->SetRange("Size>=0.");
  //  AbsThickCmd->SetUnitCategory("Length");
  //  AbsThickCmd->AvailableForStates(Idle);
  
  //  GapThickCmd = new G4UIcmdWithADoubleAndUnit("/quad/setGapThick",this);
  //  GapThickCmd->SetGuidance("Set Thickness of the Gap");
  //  GapThickCmd->SetParameterName("Size",false);
  //  GapThickCmd->SetRange("Size>=0.");
  //  GapThickCmd->SetUnitCategory("Length");  
  //  GapThickCmd->AvailableForStates(Idle);
  
  //  SizeYZCmd = new G4UIcmdWithADoubleAndUnit("/quad/setSizeYZ",this);
  //  SizeYZCmd->SetGuidance("Set tranverse size of the quadimeter");
  //  SizeYZCmd->SetParameterName("Size",false);
  //  SizeYZCmd->SetRange("Size>0.");
  //  SizeYZCmd->SetUnitCategory("Length");    
  //  SizeYZCmd->AvailableForStates(Idle);
  
  //  NbLayersCmd = new G4UIcmdWithAnInteger("/quad/setNbOfLayers",this);
  //  NbLayersCmd->SetGuidance("Set number of layers.");
  //  NbLayersCmd->SetParameterName("NbLayers",false);
  //  NbLayersCmd->SetRange("NbLayers>0 && NbLayers<500");
  //  NbLayersCmd->AvailableForStates(Idle);

  //  UpdateCmd = new G4UIcmdWithoutParameter("/quad/update",this);
  //  UpdateCmd->SetGuidance("Update quadimeter geometry.");
  //  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  //  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  //  UpdateCmd->AvailableForStates(Idle);
  /*
  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/quad/setField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Y direction.");
  MagFieldCmd->SetParameterName("By",false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(Idle);  
  */
}

//=================================================================

BDSDetectorMessenger::~BDSDetectorMessenger()
{
  //  delete NbLayersCmd;
  //  delete AbsMaterCmd; delete GapMaterCmd;
  //  delete AbsThickCmd; delete GapThickCmd;
  //  delete SizeYZCmd;   delete UpdateCmd;
  /*
  delete MagFieldCmd;
  delete BDSdetDir;
  */
}

//=================================================================

void BDSDetectorMessenger::SetNewValue(G4UIcommand* /*command*/,G4String /*newValue*/)
{ 
  //  if( command == AbsMaterCmd )
  //   { BDSDetector->SetAbsorberMaterial(newValue);}
   
  //  if( command == GapMaterCmd )
  //   { BDSDetector->SetGapMaterial(newValue);}
  
  //  if( command == AbsThickCmd )
  //   { BDSDetector->SetAbsorberThickness(AbsThickCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == GapThickCmd )
  //   { BDSDetector->SetGapThickness(GapThickCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == SizeYZCmd )
  //   { BDSDetector->SetQuadSizeYZ(SizeYZCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == NbLayersCmd )
  //   { BDSDetector->SetNbOfLayers(NbLayersCmd->GetNewIntValue(newValue));}
  
  //  if( command == UpdateCmd )
  //   { BDSDetector->UpdateGeometry(); }

  /*
  if( command == MagFieldCmd )
   { BDSDetector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}
  */
}

//=================================================================
