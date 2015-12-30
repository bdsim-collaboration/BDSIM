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

  inline G4String        GetInputFilename() const        {return inputFilename;}
  inline G4String        GetVisMacroFilename() const     {return visMacroFilename;}
  inline G4bool          GetVisDebug() const             {return visDebug;}
  inline G4String        GetOutputFilename() const       {return outputFilename;}
  inline G4bool          GetOutputFilenameSet() const    {return outputFilenameSet;}
  inline BDSOutputFormat GetOutputFormat() const         {return outputFormat;}
  inline G4bool          GetGFlash() const               {return gflash;}
  inline G4double        GetGFlashEMax() const           {return gflashemax;}
  inline G4double        GetGFlashEMin() const           {return gflashemin;}  
  inline G4bool          GetVerbose() const              {return verbose;}
  inline G4bool          GetVerboseEvent() const         {return verboseEvent;}
  inline G4bool          GetVerboseStep() const          {return verboseStep;}
  inline G4int           GetVerboseEventNumber() const   {return verboseEventNumber;}
  inline G4bool          GetBatch() const                {return batch;}
  inline G4int           GetVerboseRunLevel() const      {return verboseRunLevel;}
  inline G4int           GetVerboseEventLevel() const    {return verboseEventLevel;}
  inline G4int           GetVerboseTrackingLevel() const {return verboseTrackingLevel;}
  inline G4int           GetVerboseSteppingLevel() const {return verboseSteppingLevel;}
  inline G4bool          GetCircular() const             {return circular;}
  inline G4int           GetSeed() const                 {return seed;}
  inline G4bool          IsSeedSet() const               {return setSeed;}
  inline G4String        GetBDSIMPATH() const            {return itsBDSIMPATH;}
  inline G4bool          SetSeedState() const            {return setSeedState;}
  inline G4String        GetSeedStateFilename() const    {return seedStateFilename;}
  inline G4int           GetNGenerate() const            {return nGenerate;}
  inline G4bool          ExportGeometry() const          {return exportGeometry;}
  inline G4bool          GeneratePrimariesOnly() const   {return generatePrimariesOnly;}
  inline G4bool          GetSurvey() const               {return survey;}
  inline G4String        GetSurveyFilename() const       {return surveyFilename;}
  inline G4String        GetExportType() const           {return exportType;}
  inline G4String        GetExportFileName() const       {return exportFileName;}

protected : 
  BDSExecOptions(int argc, char** argv);
  static BDSExecOptions* _instance;
  
private :
  BDSExecOptions();

  /** Parse the command line options
   * @param[in] argc command line input number of variables
   * @param[in] argv array of char* of the input parameters
   */
  void Parse(int arcg, char **argv);
  /// Helper method to set the BDSIMPath correctly
  G4String GetPath(G4String filename);

  G4String        inputFilename;    ///< input filename
  G4String        visMacroFilename; ///< visualisation filename
  G4bool          visDebug;         ///< flag for visualisation debug
  
  ///@{ Parameter for output format
  G4String        outputFilename;
  G4bool          outputFilenameSet;
  BDSOutputFormat outputFormat;
  ///@}
  
  ///@{ Parameter for survey
  G4bool          survey;
  G4String        surveyFilename;
  ///@}
  
  ///@{ Parameter for gflash shower parameterisation
  G4bool   gflash;
  G4double gflashemax;
  G4double gflashemin;
  ///@}
  
  G4bool batch; ///< Flag for batch / interactive mode

  ///@{ Geant4 verbose levels
  G4bool verbose;
  G4bool verboseEvent;
  G4bool verboseStep;
  G4int  verboseEventNumber;

  G4int  verboseRunLevel;
  G4int  verboseEventLevel;
  G4int  verboseTrackingLevel;
  G4int  verboseSteppingLevel;
  ///@}
  
  G4bool circular; ///< Flag for circular machine
  G4int  seed;     ///< The seed value for the random number generator
  G4bool setSeed;  ///< If the seed was set by command line - override if specified in options
  /// string that points to path where files are searched; based on environment variable or else input filename
  G4String itsBDSIMPATH;
  G4bool   setSeedState;      ///< If seed state is specifed - use the state saved in it
  G4String seedStateFilename; ///< The seed state filename

  G4int    nGenerate; ///< The number of primary events to simulate

  G4bool   generatePrimariesOnly; ///< Whether to only generate primary coordinates and quit, or not.

  ///@{ Parameter for controlling geometry export
  G4bool   exportGeometry;
  G4String exportType;
  G4String exportFileName;
  ///@}
};

#endif
