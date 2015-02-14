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

/**
 * @brief a 1d-histogram class
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */
class BDSHistogram1D
{
public:
  BDSHistogram1D(G4double xmin,
		 G4double xmax,
		 G4int    nbins,
		 G4String nameIn="histogram",
		 G4String titleIn="histogram");
  BDSHistogram1D(std::vector<G4double> binEdges,
		 G4String name="histogram",
		 G4String titleIn="histogram");
  ~BDSHistogram1D();
  
  void                                        Empty();
  void                                        Fill(G4double x);
  void                                        Fill(G4double x, G4double weight);
  std::vector<BDSBin*>                        GetBins() const;
  std::vector<G4double>                       GetBinValues() const;
  std::vector<std::pair<G4double, G4double> > GetBinXMeansAndTotals() const;
  /// print value of all bins to cout
  void                                        PrintBins() const;
  std::pair<BDSBin*,BDSBin*>                  GetUnderOverFlowBins() const;
  std::pair<G4double,G4double>                GetUnderOverFlowBinValues() const;
  BDSBin*                                     GetUnderflowBin() const;
  BDSBin*                                     GetOverflowBin()  const;
  BDSBin*                                     GetFirstBin() const;
  BDSBin*                                     GetLastBin() const;
  G4String                                    GetName() const;
  G4String                                    GetTitle() const;
  size_t                                      GetNBins() const;

  // iterators
  BDSBin* currentBin();
  void    first();
  G4bool  isLastBin();
  G4bool  isDone();
  void    next();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist);
  
private:
  /// vector of bins
  /// 1st bin is underflow bin always
  /// last bin is overflow bin always
  std::vector<BDSBin*> bins;
  BDSBin*  overflow;
  BDSBin*  underflow;
  G4double binwidth;
  G4String name;
  G4String title;

  std::vector<BDSBin*>::const_iterator _iterBins;
};

#endif
