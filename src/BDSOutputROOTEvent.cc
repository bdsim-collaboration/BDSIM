#ifdef USE_ROOT

#include "BDSOutputROOTEvent.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

BDSOutputROOTEvent::BDSOutputROOTEvent() 
{  
  G4cout << __METHOD_NAME__ <<G4endl;
  this->Init(); 
}

BDSOutputROOTEvent::~BDSOutputROOTEvent() 
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

void BDSOutputROOTEvent::Init() 
{
  const BDSExecOptions*     execOptions     = BDSExecOptions::Instance();
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  // Base root file name 
  G4String basefilename = execOptions->GetOutputFilename();
  basefilename = basefilename+"_event";

  // if more than one file add number (starting at 0)
  int evntsPerNtuple = globalConstants->GetNumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && globalConstants->GetNumberToGenerate()>evntsPerNtuple)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + ".root";

  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!execOptions->GetOutputFilenameSet() || !execOptions->GetBatch()) {
    // check if file exists
    int nTimeAppended = 1;
    while (BDS::FileExists(filename)) {
      // if exists remove trailing .root
      filename = basefilename + "-" + std::to_string(nTimeAppended);
      filename += ".root";
      nTimeAppended +=1;
    }
  }

  G4cout << __METHOD_NAME__ << "Setting up new file: "<<filename<<G4endl;
  theRootOutputFile=new TFile(filename,"RECREATE", "BDS output file");
}
  
/// write sampler hit collection
void BDSOutputROOTEvent::WriteHits(BDSSamplerHitsCollection*) {}

/// write energy deposition hits
void BDSOutputROOTEvent::WriteEnergyLoss(BDSEnergyCounterHitsCollection*) {}

/// write where primaries stop being primaries
void BDSOutputROOTEvent::WritePrimaryLoss(BDSEnergyCounterHit* // ploss
					  ) {}

/// write where primaries impact
void BDSOutputROOTEvent::WritePrimaryHit(BDSEnergyCounterHit* // phits
					 ) {}

/// write tunnel hits
void BDSOutputROOTEvent::WriteTunnelHits(BDSTunnelHitsCollection* // tunnelHits
					 ) {}

/// write a trajectory 
void BDSOutputROOTEvent::WriteTrajectory(std::vector<BDSTrajectory*> &// TrajVec
					 ) {}

/// write primary hit
void BDSOutputROOTEvent::WritePrimary(G4double /*E*/,
				      G4double /*x0*/,
				      G4double /*y0*/,
				      G4double /*z0*/,
				      G4double /*xp*/,
				      G4double /*yp*/,
				      G4double /*zp*/,
				      G4double /*t*/,
				      G4double /*weight*/,
				      G4int    /*PDGType*/,
				      G4int    /*nEvent*/,
				      G4int    /*TurnsTaken*/) {}

/// write a histgoram
void BDSOutputROOTEvent::WriteHistogram(BDSHistogram1D* /*histogramIn*/) {}

/// write and close and open new file
void BDSOutputROOTEvent::Commit() {}

/// write and close the file
void BDSOutputROOTEvent::Write() {}

#endif
