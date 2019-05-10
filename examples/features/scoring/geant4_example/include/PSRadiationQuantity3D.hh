//
// Created by rtesse on 28/03/19.
//

#ifndef B1_PSRADIATIONQUANTITY3D_HH
#define B1_PSRADIATIONQUANTITY3D_HH

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include <vector>

using namespace std;



class PSRadiationQuantity3D : public G4VPrimitiveScorer
{
public: // with description

    PSRadiationQuantity3D(G4String scorer_name ="scorer", G4String filename="",
    G4int ni=1,G4int nj=1, G4int nk=1,
    G4int depi=2, G4int depj=1, G4int depk=0);

    ~PSRadiationQuantity3D() override;


protected: // with description
    G4bool ProcessHits(G4Step*,G4TouchableHistory*) override;
    G4int GetIndex(G4Step* aStep) override;

public:
    void Initialize(G4HCofThisEvent*) override;
    void EndOfEvent(G4HCofThisEvent*) override;
    void clear() override;
    inline G4double GetInterpFactor(G4double, vector<vector<G4double> > );

private:

    G4int HCID3D;
    G4THitsMap<G4double>* EvtMap3D;
    vector<vector<G4double> > conversion_factor;
    G4int fDepthi, fDepthj, fDepthk;

};


#endif //B1_PSRADIATIONQUANTITY3D_HH
