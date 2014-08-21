#ifndef BDSHistogram_h
#define BDSHistogram_h 1

#include "globals.hh"
#include <vector>
#include <utility>
//#include <string>

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


class BDSHistogram1D
{
public:
  BDSHistogram1D(G4double xmin, G4double xmax, G4int nbins);
  ~BDSHistogram1D();
  
  void                                        Empty();
  void                                        Fill(G4double x);
  void                                        Fill(G4double x, G4double weight);
  std::vector<BDSBin*>                        GetBins();
  std::vector<G4double>                       GetBinTotals();
  std::vector<std::pair<G4double, G4double> > GetBinValues();
  void                                        PrintBins();
  std::pair<G4double,G4double>                GetUnderOverFlowBins();
  G4String                                    GetInfo();
  
private:
  // vector of bins
  // 1st bin is underflow bin always
  // last bin is overflow bin always
  std::vector<BDSBin*> bins;
  G4double binwidth;
};

#endif
