#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#include "BDSOutput.hh"

#include "globals.hh"

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief ROOT Event output class.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOT: public BDSOutput 
{
public:
  /// Constructor with default file name (without extension or number suffix).
  /// Also, file number offset to start counting suffix from.
  BDSOutputROOT(G4String fileName,
		G4int    fileNumberOffset);
  virtual ~BDSOutputROOT();

  virtual void NewFile();                     ///< Open a new file.
  virtual void CloseFile();                   ///< Write contents and close file.

  /// Copy from local event structures to the actual file.  Ony event level
  /// structures are copied.
  virtual void WriteFileEventLevel();

  /// Copy from local run structures to the actual file.  Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel();

private:
  /// No default constructor.
  BDSOutputROOT() = delete;

  /// Output file.
  TFile* theRootOutputFile = nullptr;

  /// Options tree.
  TTree* theOptionsOutputTree = nullptr;

  /// Model tree.
  TTree* theModelOutputTree = nullptr;

  /// Event tree.
  TTree* theEventOutputTree = nullptr;

  /// Output histogram tree.
  TTree* theRunOutputTree  = nullptr;
};


#endif
