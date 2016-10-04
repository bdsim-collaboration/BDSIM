#ifndef BDSBIN_H
#define BDSBIN_H

#include "globals.hh" // geant4 globals / types

#include <vector>
#include <utility>
#include <ostream>

/**
 * @brief A histogram bin.
 * 
 * @author Laurie Nevay
 */

class BDSBin
{
public:
  BDSBin(G4double inXmin, G4double inXmax);
  G4double GetValue()    const {return total;}
  G4double GetError()    const {return sqrt(sumWeightsSquared);}
  G4double GetLowerEdge()const {return xmin;}
  G4double GetUpperEdge()const {return xmax;}
  G4double GetLength()   const {return xmax - xmin;}
  std::pair<G4double, G4double> GetXMeanAndTotal() const;
  BDSBin operator+= (const G4double& weight);
  G4bool operator== (const G4double& rhs   ) {return (total == rhs);}
  G4bool operator!= (const G4double& rhs   ) {return !operator == (rhs);}
  void   Empty(){total = 0; sumWeightsSquared = 0;}
  G4bool InRange(G4double x) const;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBin const &bin);

private:
  /// Private default constructor to force use of provided one.
  BDSBin();
  
  G4double xmin;
  G4double xmax;
  G4double total;
  G4double sumWeightsSquared;
};


#endif
