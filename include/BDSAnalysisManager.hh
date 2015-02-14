#ifndef BDSANALYSISMANAGER_H
#define BDSANALYSISMANAGER_H

#include "BDSHistogram.hh"
#include "globals.hh"         // geant4 globals / types
#include <vector>

/**
 * @brief Analysis interface class. Create, store and access
 * histograms.
 * 
 * singleton pattern
 * 
 * This is conceptually based on the Geant4 AnalysisManager classes,
 * however this does not inherit it nor adhere strictly to it. The
 * purpose of this class is only to deal with histograms. The Geant4
 * AnalysisManagers are based on an output format.  Here this will 
 * be a communal histogram class and storage system and the different
 * output formats will write them appropriately.
 */

class BDSAnalysisManager
{
public:
  /// singleton accessor
  static BDSAnalysisManager* Instance();
  ~BDSAnalysisManager();

  /// Create a new histogram
  G4int Create1DHistogram(G4String name,
			  G4String title,
			  G4int    nbins,
			  G4double xmin,
			  G4double xmax);

  G4int Create1DHistogram(G4String name,
			  G4String title,
			  std::vector<double>& edges);

  /// Access a histogram
  BDSHistogram1D* GetHistogram(G4int index);

  /// Fill a histogram
  void Fill1DHistogram(G4int histoIndex, G4double value, G4double weight=1.0);
  
  

private:
  /// private default constructor for singleton pattern
  BDSAnalysisManager();

  /// function to check whether histogram index is valid
  void CheckHistogramIndex(G4int index);
  
  static BDSAnalysisManager* _instance;

  std::vector<BDSHistogram1D*> histograms1d;
  
};

#endif

  
