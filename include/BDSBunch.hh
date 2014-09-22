#ifndef BDSBunch_h
#define BDSBunch_h

#include <string>
#include "BDSBunchInterface.hh"

#include "globals.hh"
#include "parser/options.h"

/**
 * @brief the particle generator
 * 
 * This class generates the particles
 *
 * @author Stewart Boogert <Stewart.Boogert@rhul.ac.uk>
 */

// can become a singleton? - JS

class BDSBunch {

protected:
  /// particle distribution
  std::string        distribType;
  /// bdsBunch holds the distribution information and provides the next particle
  BDSBunchInterface *bdsBunch; 
  
public:
  BDSBunch(); 
  ~BDSBunch(); 
  void SetOptions(struct Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight); 

  std::string        GetDistributionType() {return distribType;}
  BDSBunchInterface* GetBDSBunchInterface() {return bdsBunch;}
};

#endif
