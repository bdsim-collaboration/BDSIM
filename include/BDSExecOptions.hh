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
  static BDSExecOptions* Instance(int argc, char **argv);
  ~BDSExecOptions();

  void Usage();
  void Print(); 

  inline G4String const GetInputFilename()                   {return inputFilename;}
  inline G4String const GetVisMacroFilename()                {return visMacroFilename;}
  inline G4String const GetOutputFilename()                  {return outputFilename;}
  inline BDSOutputFormat const GetOutputFormat()             {return outputFormat;}
  inline G4bool const  GetOutline()                         {return outline;}
  inline G4String const GetOutlineFilename()                 {return outlineFilename;}
  inline G4String const GetOutlineFormat()                   {return outlineFormat;}

  inline G4int const   GetGFlash()               {return gflash;}
  inline G4double const GetGFlashEMax()           {return gflashemax;}
  inline G4double const GetGFlashEMin()           {return gflashemin;}  

  inline G4bool const  GetVerbose()              {return verbose;}
  inline G4bool const  GetVerboseEvent()         {return verboseEvent;}
  inline G4bool const  GetVerboseStep()          {return verboseStep;}
  inline G4int const   GetVerboseEventNumber()   {return verboseEventNumber;}
  inline G4bool const  GetBatch()                {return batch;}

  inline G4int  const  GetVerboseRunLevel()      {return verboseRunLevel;}
  inline G4int  const  GetVerboseEventLevel()    {return verboseEventLevel;}
  inline G4int  const  GetVerboseTrackingLevel() {return verboseTrackingLevel;}
  inline G4int  const  GetVerboseSteppingLevel() {return verboseSteppingLevel;}

  inline G4bool const  GetCircular()             {return circular;}


protected : 
  BDSExecOptions(int argc, char** argv);
  static BDSExecOptions* _instance;
  
  
private :
  BDSExecOptions();
  void Parse(int arcg, char **argv);
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

};

#endif
