#ifndef BDSBUNCHRECREATE
#define BDSBUNCHRECREATE

#include "BDSBunchInterface.hh"
#include "analysis/Event.hh"
#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

#include "TChain.h"


class BDSBunchRecreate: public BDSBunchInterface
{
public:
  BDSBunchRecreate();
  virtual ~BDSBunchRecreate();

  virtual void SetOptions(const GMAD::Options& opt);

  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

private:
  /// Open the root file and overlay the data on a local instance of the correct class.
  void LoadFile();
  
  /// File to load coordinates from
  std::string filename;

  /// Offset in event number from start of file
  G4int eventOffset;

  /// Current event number
  G4int currentEventNumber;

  /// Number of available entries in the file.
  G4int entries;

  /// If true, the seed state will also be loaded and set, if not, merely the
  /// coordinates from the file will be used. This will default to true.
  G4bool strongRecreation;
  
  /// Chain of file(s) w.r.t to Event tree for primary coordinate loading.
  TChain* chain;

  /// A local instance of the event class to copy variables from the ROOT file onto.
  Event* event;
};

#endif
