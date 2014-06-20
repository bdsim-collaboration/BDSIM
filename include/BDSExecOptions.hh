/* BDSIM code.    
   Author: Stewart T. Boogert , Royal Holloway, Univ. of London.
   Last modified 16.04.2013
   Copyright (c) 2002 by BDSIM authors.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSExecOptions_h
#define BDSExecOptions_h

#include <getopt.h>

#include "G4String.hh"
#include "G4Types.hh"
#include "BDSOutputFormat.hh"

class BDSExecOptions
{
public: 

  static BDSExecOptions* Instance();
  ~BDSExecOptions();

  void Usage();
  void Parse(int arcg, char **argv);
  void Print(); 

  G4String GetInputFilename()                   {return inputFilename;}
  void     SetInputFilename(G4String s)         {inputFilename = s;}       
  G4String GetVisMacroFilename()                {return visMacroFilename;}
  void     SetVisMacroFilename(G4String s)      {visMacroFilename = s;}
  G4String GetOutputFilename()                  {return outputFilename;}
  void     SetOutputFilename(G4String s)        {outputFilename = s;}
  BDSOutputFormat GetOutputFormat()             {return outputFormat;}
  void     SetOutputFormat(BDSOutputFormat f)   {outputFormat = f;}
  G4bool   GetOutline()                         {return outline;}
  void     SetOutline(G4bool b)                 {outline = b;}
  G4String GetOutlineFilename()                 {return outlineFilename;}
  void     SetOutlineFilename(G4String s)       {outlineFilename = s;}
  G4String GetOutlineFormat()                   {return outlineFormat;}
  void     SetOutlineFormat(G4String s)         {outlineFormat = s;}

  G4int    GetGFlash()               {return gflash;}
  void     SetGFlash(G4bool b)       {gflash = b;}
  G4double GetGFlashEMax()           {return gflashemax;}
  void     SetGFlashEMax(G4double d) {gflashemax = d;}
  G4double GetGFlashEMin()           {return gflashemin;}  
  void     SetGFlashEMin(G4double d) {gflashemin = d;}

  G4int    GetNPTwiss()              {return nptwiss;}
  void     SetNPTwiss(G4int i)       {nptwiss = i;}
  
  G4bool   GetVerbose()              {return verbose;}
  G4bool   GetVerboseEvent()         {return verboseEvent;}
  G4bool   GetVerboseStep()          {return verboseStep;}
  G4int    GetVerboseEventNumber()   {return verboseEventNumber;}
  void     SetVerboseEventNumber(G4int val)   {verboseEventNumber = val;}
  G4bool   GetBatch()                {return batch;}

  G4int    GetVerboseRunLevel()      {return verboseRunLevel;}
  G4int    GetVerboseEventLevel()    {return verboseEventLevel;}
  G4int    GetVerboseTrackingLevel() {return verboseTrackingLevel;}
  G4int    GetVerboseSteppingLevel() {return verboseSteppingLevel;}

  G4bool   GetCircular()             {return circular;}


protected : 
  BDSExecOptions();
  static BDSExecOptions* _instance;

  
private :
  G4String        inputFilename;
  G4String        visMacroFilename;
  G4String        outputFilename;
  BDSOutputFormat outputFormat;
  G4bool          outline;
  G4String        outlineFilename;
  G4String        outlineFormat;

  G4int    gflash;
  G4double gflashemax;
  G4double gflashemin;

  G4int    nptwiss;

  G4bool verbose;
  G4bool verboseEvent;
  G4bool verboseStep;
  G4int  verboseEventNumber;
  G4bool batch; 
  G4bool listMaterials;
  
  G4int  verboseRunLevel;
  G4int  verboseEventLevel;
  G4int  verboseTrackingLevel;
  G4int  verboseSteppingLevel;

  G4bool circular;

};

#endif
