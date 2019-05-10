#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "globals.hh"
#include "G4THitsMap.hh"

#include "G4Event.hh"

/// Run class
///

class Run : public G4Run
{
  public:
    Run(const std::vector<G4String> mfdName);
    virtual ~Run();


  public:

    // virtual method from G4Run.
  // The method is overriden in this class for scoring.
  virtual void RecordEvent(const G4Event*);

  // Access methods for scoring information.
  // - Number of HitsMap for this RUN.
  //   This is equal to number of collections.
  G4int GetNumberOfHitsMap() const {return fRunMap.size();}
  // - Get HitsMap of this RUN.
  //   by sequential number, by multifucntional name and collection name,
  //   and by collection name with full path.
  G4THitsMap<G4double>* GetHitsMap(G4int i){return fRunMap[i];}
  G4THitsMap<G4double>* GetHitsMap(const G4String& detName,
                                  const G4String& colName);
  G4THitsMap<G4double>* GetHitsMap(const G4String& fullName);
  // - Dump All HitsMap of this RUN.
  //   This method calls G4THisMap::PrintAll() for individual HitsMap.
  void DumpAllScorer();

    // method from the base class
    virtual void Merge(const G4Run*);

  private:

  std::vector<G4String> fCollName;
  std::vector<G4int> fCollID;
  std::vector<G4THitsMap<G4double>*> fRunMap;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

