#ifndef BDSBIN_H
#define BDSBIN_H

#include "globals.hh"       // geant4 globals / types
#include <vector>
#include <utility>
#include <ostream>

/**
 * @brief A histogram bin.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */
class BDSBin
{
public:
  BDSBin(G4double inXmin, G4double inXmax);
  G4double GetValue(){return total;}
  G4double GetError(){return sqrt(sumWeightsSquared);}
  G4double GetLowerEdge(){return xmin;}
  G4double GetUpperEdge(){return xmax;}
  std::pair<G4double, G4double> GetXMeanAndTotal();
  BDSBin operator+= (const G4double& weight);
  G4bool operator== (const G4double& rhs   ) {return (total == rhs);}
  G4bool operator!= (const G4double& rhs   ) {return !operator == (rhs);}
  void   Empty(){total = 0; sumWeightsSquared = 0;}
  G4bool InRange(G4double x);

  G4double xmin;
  G4double xmax;
  G4double xmean;
  G4double total;
  G4double sumWeightsSquared;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBin const &bin);
};


#endif
