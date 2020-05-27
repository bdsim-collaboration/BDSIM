//
// Created by Eliott Ramoisiaux on 26/05/2020.
//

#ifndef BDSIM_BDSHISTBINMAPPER_HH
#define BDSIM_BDSHISTBINMAPPER_HH

#include "G4ThreeVector.hh"


class BDSHistBinMapper {
public:
    BDSHistBinMapper(G4int nBinsIIn,G4int nBinsJIn,G4int nBinsKIn, G4int nBinsLIn);

    virtual ~BDSHistBinMapper(){;}

    virtual G4int GlobalFromIJKLIndex(G4int iIndex,
                                     G4int jIndex,
                                     G4int kIndex,
                                     G4int lIndex = 1) const;

    virtual void IJKLFromGlobal(G4int  globalBin,
                               G4int& iIndex,
                               G4int& jIndex,
                               G4int& kIndex,
                               G4int& lIndex) const;

    inline G4double NBinsI() const {return nBinsI;}
    inline G4double NBinsJ() const {return nBinsJ;}
    inline G4double NBinsK() const {return nBinsK;}
    inline G4double NBinsL() const {return nBinsL;}

protected:
    G4int nBinsI;
    G4int nBinsJ;
    G4int nBinsK;
    G4int nBinsL;
};


#endif //BDSIM_BDSHISTBINMAPPER_HH
