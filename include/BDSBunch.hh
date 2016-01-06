#ifndef BDSBUNCH_H
#define BDSBUNCH_H

#include <string>
#include "BDSBunchInterface.hh"

#include "globals.hh"
#include "parser/options.h"

/**
 * @brief A delegator class for the BDSIM particle distribution generator.
 * 
 * The user should instantiate this class only and set the options from the parser.
 * This class generates new particle coordinates for each event based on the 
 * GMAD::Options structure passed from the parser. 
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
  void SetOptions(const GMAD::Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight); 

  std::string        GetDistributionType() {return distribType;}
  BDSBunchInterface* GetBDSBunchInterface() {return bdsBunch;}
};

#endif
