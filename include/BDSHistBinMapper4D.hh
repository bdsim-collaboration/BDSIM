//
// Created by Eliott Ramoisiaux on 25/05/2020.
//

#ifndef BDSIM_BDSHISTBINMAPPER4D_HH
#define BDSIM_BDSHISTBINMAPPER4D_HH

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "BDSHistBinMapper3D.hh"

class BDSHistBinMapper4D : public BDSHistBinMapper3D{

public:
    BDSHistBinMapper4D(G4int nBinsIIn,G4int nBinsJIn,G4int nBinsKIn,G4int nBinsLIn);

    virtual ~BDSHistBinMapper4D(){;}

    virtual G4int GlobalFromIJKLIndex(G4int iIndex,
                                     G4int jIndex,
                                     G4int kIndex,
                                     G4int lIndex) const;

    virtual void IJKLFromGlobal(G4int  globalBin,
                               G4int& iIndex,
                               G4int& jIndex,
                               G4int& kIndex,
                               G4int& lIndex) const;

    inline G4double NBinsL() const {return nBinsL;}
    //inline G4ThreeVector NBins() const {return G4ThreeVector(nBinsI, nBinsJ, nBinsK, nBinsL);}

protected:
    G4int nBinsL;
};


#endif //BDSIM_BDSHISTBINMAPPER4D_HH
