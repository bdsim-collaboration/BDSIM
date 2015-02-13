#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSHistogram.hh"
#include "globals.hh"             // geant4 globals / types

#include <vector>
#include <iterator>

BDSAnalysisManager* BDSAnalysisManager::_instance = 0;

BDSAnalysisManager* BDSAnalysisManager::Instance()
{
  if (_instance == 0)
    {
      _instance = new BDSAnalysisManager();
    }
  return _instance;
}

BDSAnalysisManager::BDSAnalysisManager(){;}

BDSAnalysisManager::~BDSAnalysisManager()
{
  //delete all the 1d histograms created
  std::vector<BDSHistogram1D*>::iterator iter, end;
  for (iter = histograms1d.begin(), end = histograms1d.end(); iter != end; ++iter)
    {
      delete (*iter);
    }
}

G4int BDSAnalysisManager::Create1DHistogram(G4String name,
					    G4String /*title*/,
					    G4int    nbins,
					    G4double xmin,
					    G4double xmax)
{
  histograms1d.push_back(new BDSHistogram1D(xmin, xmax, nbins, name));
  return histograms1d.size();
}

G4int BDSAnalysisManager::Create1DHistogram(G4String name,
					    G4String /*title*/,
					    std::vector<double>& edges)
{
  histograms1d.push_back(new BDSHistogram1D(edges,name));
  return histograms1d.size();
}

BDSHistogram1D* BDSAnalysisManager::GetHistogram(G4int histoIndex)
{
  CheckHistogramIndex(histoIndex);
  return histograms1d[histoIndex];
}

void BDSAnalysisManager::Fill1DHistogram(G4int histoIndex, G4double value, G4double weight)
{
  CheckHistogramIndex(histoIndex);
  histograms1d[histoIndex]->Fill(value,weight);
}

void  BDSAnalysisManager::CheckHistogramIndex(G4int histoIndex)
{
  if ( ((size_t)histoIndex > histograms1d.size()) || histoIndex < 0 )
  {
    G4cerr << __METHOD_NAME__ << "invalid histogram index" << G4endl;
    exit(1);
  }
  // void - keep going
}
