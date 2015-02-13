#ifndef BDSHistogram_h
#define BDSHistogram_h 1

#include "globals.hh"       // geant4 globals / types
#include <vector>
#include <utility>
#include <ostream>

/**
 * @brief a histogram bin
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */
class BDSBin
{
public:
  BDSBin(G4double inXmin, G4double inXmax);
  G4double GetValue(){return total;}
  std::pair<G4double, G4double> GetXMeanAndTotal();
  BDSBin operator+= (const G4double& weight) {total += weight; return *this;}
  G4bool operator== (const G4double& rhs   ) {return (total == rhs);}
  G4bool operator!= (const G4double& rhs   ) {return !operator == (rhs);}
  void   Empty(){total = 0;}
  G4bool InRange(G4double x);

  G4double xmin;
  G4double xmax;
  G4double xmean;
  G4double total;
};

/**
 * @brief a 1d-histogram class
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */
class BDSHistogram1D
{
public:
  BDSHistogram1D(G4double xmin, G4double xmax, G4int nbins, G4String nameIn="histogram");
  BDSHistogram1D(std::vector<G4double> binEdges, G4String name="histogram");
  ~BDSHistogram1D();
  
  void                                        Empty();
  void                                        Fill(G4double x);
  void                                        Fill(G4double x, G4double weight);
  std::vector<BDSBin*>                        GetBins() const;
  std::vector<G4double>                       GetBinTotals() const;
  std::vector<std::pair<G4double, G4double> > GetBinValues() const;
  /// print value of all bins to cout
  void                                        PrintBins() const;
  std::pair<G4double,G4double>                GetUnderOverFlowBins() const;
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist);
  
private:
  /// vector of bins
  /// 1st bin is underflow bin always
  /// last bin is overflow bin always
  std::vector<BDSBin*> bins;
  G4double binwidth;
  G4String name;
};

#endif
