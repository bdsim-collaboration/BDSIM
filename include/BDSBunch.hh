// Provide interface to generating
// the primary particles


#ifndef BDSBunch_h
#define BDSBunch_h 

// BDSIM
#include "parser/gmad.h"
#include "globals.hh"

// CLHEP vector
#include "Randomize.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

// C++ 
#include <fstream>
#include <list>

// CLHEP < 1.9
//class RandGauss;
//class RandFlat;


struct Doublet {

  G4String name;
  G4double unit; // relative to SI units, i.e. mm=0.001 etc.

};

class BDSBunch {

public: 
  BDSBunch(); // default constructor
  ~BDSBunch();

public:

  void SetOptions(struct Options&);

  G4double GetSigmaT();  // get initial bunch distribution parameters in Gaussian case 
  G4double GetSigmaX(); 
  G4double GetSigmaY(); 
  G4double GetSigmaXp();
  G4double GetSigmaYp();

  // get next coordinate 
  G4double GetNextX(); 
  G4double GetNextY(); 
  G4double GetNextZ(); 
  G4double GetNextXp();
  G4double GetNextYp();
  G4double GetNextT(); 

  // the same in a bulk
  void GetNextParticle(G4double& x0,G4double& y0,G4double& z0,
		       G4double& xp,G4double& yp,G4double& zp,
		       G4double& t, G4double& E, G4double &weight);
  

  G4double GetEmitX();
  G4double GetEmitY();

  G4double GetAlphaX();
  G4double GetAlphaY();

  G4double GetBetaX();
  G4double GetBetaY();

  // set initial bunch distribution parameters in Gaussian case 
  void SetSigmaT(double);  
  void SetSigmaX(double); 
  void SetSigmaY(double); 
  void SetSigmaXp(double);
  void SetSigmaYp(double);

  void SetX0(double); 
  void SetY0(double); 
  void SetXp0(double);
  void SetYp0(double);

  void SetEmitX(double);
  void SetEmitY(double);

  void SetAlphaX(double);
  void SetAlphaY(double);

  void SetBetaX(double);
  void SetBetaY(double);

  void SetEnergySpread(double);
  
private:
  // options
  G4bool verbose;
  G4bool verboseStep;
  G4bool verboseEvent;
  G4bool verboseEventNumber;
  G4int  nptwiss;

  // distribution type
  int distribType;

  // distribution centre
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double T0;

  G4double Xp0;
  G4double Yp0;
  G4double Zp0;

  // parameters for Gaussian distribution
  G4double sigmaX; // sigmas for Gaussian bunches (m)
  G4double sigmaY;
  G4double sigmaT;
  G4double sigmaXp;
  G4double sigmaYp;
  
  // parameters for ring distribution
  G4double rMin;
  G4double rMax;

  // parameters for the elliptic shell distribution
  G4double shellx, shelly, shellxp, shellyp;
  
  // twiss parameters
  G4double betaX;
  G4double betaY;
  G4double alphaX;
  G4double alphaY;
  G4double emitX;
  G4double emitY;

  // sigma matrix parameters 
  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

  // energy spread
  G4double energySpread;

  std::list<struct Doublet> fields;

  // input filename
  G4String inputfile;
  G4int nlinesIgnore;
  void skip(G4int nvalues);
  std::ifstream InputBunchFile;
  template <typename Type> G4bool ReadValue(Type &value);
  void OpenBunchFile();
  void CloseBunchFile();

 // Gaussian Random number generator:
  CLHEP::RandGauss*      GaussGen;
  CLHEP::RandFlat*       FlatGen;
  CLHEP::RandMultiGauss* GaussMultiGen;


  // event number for phase for ellipse for SR rescaling
  G4int partId;
};

#endif
