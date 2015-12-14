#ifndef BDSOutputASCII_h
#define BDSOutputASCII_h 

#include "BDSOutputBase.hh"

#include <fstream>
#include "BDSHistogram.hh"

// ASCII output class

class BDSOutputASCII : public BDSOutputBase {

public: 

  BDSOutputASCII(); // default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputASCII();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*);
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec);
  /// write primary hit
  virtual void WritePrimary(G4String samplerName, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double zp,G4double tLocal,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken);

  virtual void Commit();  /// write, close and open new file
  virtual void Write();   
  virtual void Close();     

private:

  /// main output file
  std::ofstream* ofMain;
  /// primaries output file
  std::ofstream* ofPrimaries;
  /// energy loss hits output file
  std::ofstream* ofELoss;
  /// energy loss histogram
  std::ofstream* ofELossHistogram;
  // output file container
  std::vector<std::ofstream*> outputFiles;

  void initialiseFile(std::ofstream* file, 
		      std::string filename, 
		      std::string headerstring1, 
		      std::string headerstring2);

  std::string _timestring;

  BDSHistogram1D* hist;

  void WriteAsciiHit(std::ofstream* outfile, 
		     G4int    PDGType, 
		     G4double Mom, 
		     G4double X, 
		     G4double Y,
		     G4double Z,
		     G4double S, 
		     G4double XPrime, 
		     G4double YPrime, 
		     G4int    EventNo, 
		     G4double Weight, 
		     G4int    ParentID, 
		     G4int    TrackID, 
		     G4int    TurnsTaken);

  void WriteAsciiHit(std::ofstream* outfile, 
		     G4int    PDGType, 
		     G4double Mom, 
		     G4double X, 
		     G4double Y,
		     G4double Z,
		     G4double S, 
		     G4double tLocal, 
		     G4double XPrime, 
		     G4double YPrime, 
		     G4int    EventNo, 
		     G4double Weight, 
		     G4int    ParentID, 
		     G4int    TrackID, 
		     G4int    TurnsTaken);

  void WriteAsciiHit(std::ofstream* outfile, 
		     G4int PDGType, 
		     G4double Mom, 
		     G4double X, 
		     G4double Y, 
		     G4double Z, 
		     G4double XRel, 
		     G4double YRel, 
		     G4double ZRel, 
		     G4double TRel, 
		     G4double XGlobal,
		     G4double YGlobal, 
		     G4double ZGlobal, 
		     G4double tLocal,
		     G4double S, 
		     G4double SReference, 
		     G4double XPrime, 
		     G4double YPrime, 
		     G4double XPrimeGlobal, 
		     G4double YPrimeGlobal, 
		     G4int EventNo, 
		     G4double Weight, 
		     G4int ParentID, 
		     G4int TrackID, 
		     G4int TurnsTaken);

  void WriteHistogram();

};

extern BDSOutputBase* bdsOutput;
#endif
