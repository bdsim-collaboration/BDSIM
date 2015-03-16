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

  static const BDSExecOptions* Instance();
  static const BDSExecOptions* Instance(int argc, char **argv);
  ~BDSExecOptions();

  void Usage()const;
  void Print()const;

  inline G4String GetInputFilename() const                   {return inputFilename;}
  inline G4String GetVisMacroFilename() const                {return visMacroFilename;}
  inline G4String GetOutputFilename() const                  {return outputFilename;}
  inline BDSOutputFormat GetOutputFormat() const             {return outputFormat;}
  inline G4bool   GetOutline() const                         {return outline;}
  inline G4String GetOutlineFilename() const                 {return outlineFilename;}
  inline G4String GetOutlineFormat() const                   {return outlineFormat;}

  inline G4int    GetGFlash() const               {return gflash;}
  inline G4double GetGFlashEMax() const           {return gflashemax;}
  inline G4double GetGFlashEMin() const           {return gflashemin;}  

  inline G4bool  GetVerbose() const              {return verbose;}
  inline G4bool  GetVerboseEvent() const         {return verboseEvent;}
  inline G4bool  GetVerboseStep() const          {return verboseStep;}
  inline G4int   GetVerboseEventNumber() const   {return verboseEventNumber;}
  inline G4bool  GetBatch() const                {return batch;}

  inline G4int   GetVerboseRunLevel() const      {return verboseRunLevel;}
  inline G4int   GetVerboseEventLevel() const    {return verboseEventLevel;}
  inline G4int   GetVerboseTrackingLevel() const {return verboseTrackingLevel;}
  inline G4int   GetVerboseSteppingLevel() const {return verboseSteppingLevel;}

  inline G4bool  GetCircular() const             {return circular;}
  inline G4int   GetSeed() const                 {return seed;}
  inline G4bool  SetSeed() const                 {return setSeed;}
  inline G4String GetBDSIMPATH() const           {return itsBDSIMPATH;}
  inline G4bool  SetSeedState() const            {return setSeedState;}
  inline G4String GetSeedStateFilename() const   {return seedStateFilename;}

protected : 
  BDSExecOptions(int argc, char** argv);
  static BDSExecOptions* _instance;
  
  
private :
  BDSExecOptions();
  void Parse(int arcg, char **argv);
  void SetBDSIMPATH();
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
  G4int  seed; // the seed value for the random number generator
  G4bool setSeed; // if the seed was set by command line - override if specified in options
  /// string that points to path where files are searched; based on environment variable or else input filename
  G4String itsBDSIMPATH;
  G4bool   setSeedState; // if seed state is specifed - use the state saved in it
  G4String seedStateFilename; // the seed state filename
};

#endif
