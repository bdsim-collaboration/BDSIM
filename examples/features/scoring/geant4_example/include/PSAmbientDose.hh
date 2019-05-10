#ifndef PSAmbientDose_h
#define PSAmbientDose_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include <vector>

class G4VSolid;

using namespace std;


class PSAmbientDose : public G4VPrimitiveScorer
{
public: // with description
    PSAmbientDose(G4String name,G4int depth=0);
    virtual ~PSAmbientDose();


protected: // with description
    virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);

public:
    virtual void Initialize(G4HCofThisEvent*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    virtual void clear();
    inline G4double GetInterpFactor(G4double, vector<vector<G4double> > );


private:

    G4int HCID;
    G4THitsMap<G4double>* EvtMap;
    vector<vector<G4double> > conversion_factor_protons;
    vector<vector<G4double> > conversion_factor_neutrons;
    vector<vector<G4double> > conversion_factor_photons;
    vector<vector<G4double> > conversion_factor_electrons;



};
#endif

