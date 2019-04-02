//
// Created by rtesse on 27/03/19.
//

#ifndef B1_PSAMBIENTDOSE3D_HH
#define B1_PSAMBIENTDOSE3D_HH

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include <vector>

class G4VSolid;

using namespace std;

class PSAmbientDose3D : public G4VPrimitiveScorer
{
public: // with description
    PSAmbientDose3D(G4String name,
                    G4int ni=1,G4int nj=1, G4int nk=1,
                    G4int depi=2, G4int depj=1, G4int depk=0);
    virtual ~PSAmbientDose3D();


protected: // with description
    virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);
    virtual G4int GetIndex(G4Step* aStep);

public:
    virtual void Initialize(G4HCofThisEvent*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    virtual void clear();
    inline G4double GetInterpFactor(G4double, vector<vector<G4double> > );


private:

    G4int HCID3D;
    G4THitsMap<G4double>* EvtMap3D;
    vector<vector<G4double> > conversion_factor_protons;
    vector<vector<G4double> > conversion_factor_neutrons;
    vector<vector<G4double> > conversion_factor_photons;
    vector<vector<G4double> > conversion_factor_electrons;
    G4int fDepthi, fDepthj, fDepthk;

};


#endif //B1_PSAMBIENTDOSE3D_HH
